import * as vscode from 'vscode';
import * as path from 'path';
import { spawn } from 'child_process';
import { CHTLFormatter } from './formatter';
import { CHTLPreviewProvider } from './preview';
import { CHTLCompletionProvider } from './completion';
import { CHTLHoverProvider } from './hover';
import { CHTLDiagnosticsProvider } from './diagnostics';

let diagnosticCollection: vscode.DiagnosticCollection;
let previewPanel: vscode.WebviewPanel | undefined;

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL Language Support is now active!');

    // Create diagnostic collection
    diagnosticCollection = vscode.languages.createDiagnosticCollection('chtl');
    context.subscriptions.push(diagnosticCollection);

    // Register formatter
    const formatter = new CHTLFormatter();
    context.subscriptions.push(
        vscode.languages.registerDocumentFormattingEditProvider('chtl', formatter)
    );

    // Register completion provider
    const completionProvider = new CHTLCompletionProvider();
    context.subscriptions.push(
        vscode.languages.registerCompletionItemProvider(
            'chtl',
            completionProvider,
            '.', '{', '[', '@', '<', ':', '"', "'"
        )
    );

    // Register hover provider
    const hoverProvider = new CHTLHoverProvider();
    context.subscriptions.push(
        vscode.languages.registerHoverProvider('chtl', hoverProvider)
    );

    // Register diagnostics provider
    const diagnosticsProvider = new CHTLDiagnosticsProvider(diagnosticCollection);
    context.subscriptions.push(diagnosticsProvider);

    // Register commands
    context.subscriptions.push(
        vscode.commands.registerCommand('chtl.compile', compileCurrentFile),
        vscode.commands.registerCommand('chtl.preview', openPreview),
        vscode.commands.registerCommand('chtl.format', formatDocument)
    );

    // Watch for file changes
    const watcher = vscode.workspace.createFileSystemWatcher('**/*.chtl');
    watcher.onDidChange(uri => {
        if (getConfig('preview.autoRefresh') && previewPanel) {
            refreshPreview(uri);
        }
    });
    context.subscriptions.push(watcher);

    // Syntax validation on save
    vscode.workspace.onDidSaveTextDocument(document => {
        if (document.languageId === 'chtl') {
            validateCHTLDocument(document);
        }
    });
}

export function deactivate() {
    if (previewPanel) {
        previewPanel.dispose();
    }
}

async function compileCurrentFile() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showErrorMessage('No CHTL file is currently open');
        return;
    }

    const document = editor.document;
    const filePath = document.fileName;
    const outputDir = getConfig('compiler.outputDirectory');
    const compilerPath = getConfig('compiler.path');
    
    // Save the document first
    await document.save();

    // Show progress
    vscode.window.withProgress({
        location: vscode.ProgressLocation.Notification,
        title: "Compiling CHTL file...",
        cancellable: false
    }, async (progress) => {
        return new Promise<void>((resolve, reject) => {
            const args = [filePath, '-o', path.join(outputDir, path.basename(filePath, '.chtl') + '.html')];
            
            // Add compiler options
            if (getConfig('compiler.sourceMap')) {
                args.push('--source-map');
            }
            
            const optimization = getConfig('compiler.optimization');
            if (optimization !== 'none') {
                args.push(`--optimize=${optimization}`);
            }
            
            if (getConfig('compiler.parallel')) {
                args.push('--parallel');
            }

            const compiler = spawn(compilerPath, args);
            let stderr = '';

            compiler.stderr.on('data', (data) => {
                stderr += data.toString();
            });

            compiler.on('close', (code) => {
                if (code === 0) {
                    vscode.window.showInformationMessage('CHTL compilation successful!');
                    resolve();
                } else {
                    vscode.window.showErrorMessage(`CHTL compilation failed: ${stderr}`);
                    reject(new Error(stderr));
                }
            });

            compiler.on('error', (error) => {
                vscode.window.showErrorMessage(`Failed to run CHTL compiler: ${error.message}`);
                reject(error);
            });
        });
    });
}

async function openPreview() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showErrorMessage('No CHTL file is currently open');
        return;
    }

    // Compile first
    await compileCurrentFile();

    // Create or reveal preview panel
    if (previewPanel) {
        previewPanel.reveal(vscode.ViewColumn.Two);
    } else {
        previewPanel = vscode.window.createWebviewPanel(
            'chtlPreview',
            'CHTL Preview',
            vscode.ViewColumn.Two,
            {
                enableScripts: true,
                retainContextWhenHidden: true
            }
        );

        previewPanel.onDidDispose(() => {
            previewPanel = undefined;
        });
    }

    // Load compiled HTML
    const htmlPath = path.join(
        getConfig('compiler.outputDirectory'),
        path.basename(editor.document.fileName, '.chtl') + '.html'
    );

    try {
        const fs = require('fs');
        const htmlContent = fs.readFileSync(htmlPath, 'utf8');
        previewPanel.webview.html = htmlContent;
    } catch (error) {
        vscode.window.showErrorMessage(`Failed to load preview: ${error}`);
    }
}

async function formatDocument() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        return;
    }

    const formatter = new CHTLFormatter();
    const edits = await formatter.provideDocumentFormattingEdits(
        editor.document,
        { tabSize: getConfig('format.indentSize'), insertSpaces: true },
        new vscode.CancellationTokenSource().token
    );

    if (edits && edits.length > 0) {
        const workspaceEdit = new vscode.WorkspaceEdit();
        workspaceEdit.set(editor.document.uri, edits);
        await vscode.workspace.applyEdit(workspaceEdit);
    }
}

function refreshPreview(uri: vscode.Uri) {
    if (!previewPanel) {
        return;
    }

    // Re-compile and refresh
    vscode.commands.executeCommand('chtl.compile').then(() => {
        if (previewPanel) {
            const htmlPath = path.join(
                getConfig('compiler.outputDirectory'),
                path.basename(uri.fsPath, '.chtl') + '.html'
            );

            try {
                const fs = require('fs');
                const htmlContent = fs.readFileSync(htmlPath, 'utf8');
                previewPanel.webview.html = htmlContent;
            } catch (error) {
                console.error('Failed to refresh preview:', error);
            }
        }
    });
}

function validateCHTLDocument(document: vscode.TextDocument) {
    if (document.languageId !== 'chtl') {
        return;
    }

    const diagnostics: vscode.Diagnostic[] = [];
    const text = document.getText();
    const lines = text.split('\n');

    // Basic validation rules
    let braceCount = 0;
    let inString = false;
    let stringChar = '';

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];
        
        for (let j = 0; j < line.length; j++) {
            const char = line[j];
            const prevChar = j > 0 ? line[j - 1] : '';

            // String handling
            if ((char === '"' || char === "'") && prevChar !== '\\') {
                if (!inString) {
                    inString = true;
                    stringChar = char;
                } else if (char === stringChar) {
                    inString = false;
                }
            }

            // Brace counting
            if (!inString) {
                if (char === '{') {
                    braceCount++;
                } else if (char === '}') {
                    braceCount--;
                    if (braceCount < 0) {
                        const range = new vscode.Range(i, j, i, j + 1);
                        const diagnostic = new vscode.Diagnostic(
                            range,
                            'Unexpected closing brace',
                            vscode.DiagnosticSeverity.Error
                        );
                        diagnostics.push(diagnostic);
                    }
                }
            }
        }

        // Check for common patterns
        if (line.includes('class:') && !line.includes('"')) {
            const match = line.match(/class:\s*([^;]+)/);
            if (match) {
                const range = new vscode.Range(i, line.indexOf(match[1]), i, line.indexOf(match[1]) + match[1].length);
                const diagnostic = new vscode.Diagnostic(
                    range,
                    'Class attribute value should be quoted',
                    vscode.DiagnosticSeverity.Warning
                );
                diagnostics.push(diagnostic);
            }
        }
    }

    // Check for unclosed braces
    if (braceCount > 0) {
        const lastLine = lines.length - 1;
        const range = new vscode.Range(lastLine, 0, lastLine, lines[lastLine].length);
        const diagnostic = new vscode.Diagnostic(
            range,
            `Missing ${braceCount} closing brace(s)`,
            vscode.DiagnosticSeverity.Error
        );
        diagnostics.push(diagnostic);
    }

    diagnosticCollection.set(document.uri, diagnostics);
}

function getConfig(key: string): any {
    return vscode.workspace.getConfiguration('chtl').get(key);
}
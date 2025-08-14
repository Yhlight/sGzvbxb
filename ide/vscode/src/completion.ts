import * as vscode from 'vscode';

export class CHTLCompletionProvider implements vscode.CompletionItemProvider {
    private htmlTags = [
        'html', 'head', 'body', 'div', 'span', 'p', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6',
        'a', 'img', 'ul', 'ol', 'li', 'table', 'tr', 'td', 'th', 'form', 'input',
        'button', 'select', 'option', 'textarea', 'label', 'header', 'footer',
        'nav', 'section', 'article', 'aside', 'main'
    ];

    private chtlKeywords = [
        'Template', 'Custom', 'Import', 'Configuration', 'Namespace', 'Export', 'Origin',
        'style', 'script', 'text', 'insert', 'inherit', 'delete', 'except', 'from', 'as'
    ];

    private chtlTypes = [
        '@Element', '@Style', '@Var', '@Html', '@JavaScript', '@Chtl', '@CJmod'
    ];

    private cssProperties = [
        'color', 'background', 'margin', 'padding', 'border', 'width', 'height',
        'display', 'position', 'top', 'left', 'right', 'bottom', 'font-size',
        'font-family', 'text-align', 'line-height', 'flex', 'grid', 'opacity',
        'transform', 'transition', 'animation'
    ];

    private chtlJsFunctions = [
        'listen', 'delegate', 'animate'
    ];

    provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken,
        context: vscode.CompletionContext
    ): vscode.ProviderResult<vscode.CompletionItem[]> {
        const lineText = document.lineAt(position).text;
        const linePrefix = lineText.substring(0, position.character);
        const items: vscode.CompletionItem[] = [];

        // Check context
        const inStyleBlock = this.isInStyleBlock(document, position);
        const inScriptBlock = this.isInScriptBlock(document, position);
        const afterBracket = linePrefix.endsWith('[');
        const afterAt = linePrefix.endsWith('@');
        const afterDot = linePrefix.endsWith('.');
        const afterArrow = linePrefix.endsWith('->');
        const afterDoubleOpenBrace = linePrefix.endsWith('{{');

        // CHTL directives
        if (afterBracket) {
            this.chtlKeywords.forEach(keyword => {
                const item = new vscode.CompletionItem(keyword, vscode.CompletionItemKind.Keyword);
                item.detail = `CHTL ${keyword} directive`;
                item.insertText = new vscode.SnippetString(`${keyword}] $0`);
                items.push(item);
            });
        }

        // CHTL types
        if (afterAt) {
            this.chtlTypes.forEach(type => {
                const item = new vscode.CompletionItem(type.substring(1), vscode.CompletionItemKind.Class);
                item.detail = `CHTL ${type} type`;
                item.insertText = type.substring(1);
                items.push(item);
            });
        }

        // CHTL JS selector
        if (afterDoubleOpenBrace) {
            const selectorItem = new vscode.CompletionItem('selector', vscode.CompletionItemKind.Variable);
            selectorItem.detail = 'CHTL JS selector';
            selectorItem.insertText = new vscode.SnippetString('${1:selector}}}');
            items.push(selectorItem);
        }

        // CHTL JS functions
        if (afterDot || afterArrow) {
            this.chtlJsFunctions.forEach(func => {
                const item = new vscode.CompletionItem(func, vscode.CompletionItemKind.Method);
                item.detail = `CHTL JS ${func} function`;
                
                if (func === 'listen') {
                    item.insertText = new vscode.SnippetString(`${func}({\n\t\${1:click}: () => {\n\t\t$0\n\t}\n})`);
                } else if (func === 'animate') {
                    item.insertText = new vscode.SnippetString(`${func}({\n\t\${1:opacity}: [\${2:0}, \${3:1}]\n}, {\n\tduration: \${4:300}\n})`);
                } else {
                    item.insertText = new vscode.SnippetString(`${func}($0)`);
                }
                
                items.push(item);
            });
        }

        // CSS properties in style blocks
        if (inStyleBlock && !linePrefix.trim().endsWith(':')) {
            this.cssProperties.forEach(prop => {
                const item = new vscode.CompletionItem(prop, vscode.CompletionItemKind.Property);
                item.detail = 'CSS property';
                item.insertText = new vscode.SnippetString(`${prop}: \${1:value};`);
                items.push(item);
            });
        }

        // HTML tags
        if (!inStyleBlock && !inScriptBlock && !afterBracket && !afterAt) {
            this.htmlTags.forEach(tag => {
                const item = new vscode.CompletionItem(tag, vscode.CompletionItemKind.Keyword);
                item.detail = 'HTML element';
                item.insertText = new vscode.SnippetString(`${tag} {\n\t$0\n}`);
                items.push(item);
            });

            // CHTL special keywords
            const textItem = new vscode.CompletionItem('text', vscode.CompletionItemKind.Keyword);
            textItem.detail = 'CHTL text content';
            textItem.insertText = new vscode.SnippetString('text "${1:content}"');
            items.push(textItem);

            const styleItem = new vscode.CompletionItem('style', vscode.CompletionItemKind.Keyword);
            styleItem.detail = 'CHTL style block';
            styleItem.insertText = new vscode.SnippetString('style {\n\t$0\n}');
            items.push(styleItem);

            const scriptItem = new vscode.CompletionItem('script', vscode.CompletionItemKind.Keyword);
            scriptItem.detail = 'CHTL script block';
            scriptItem.insertText = new vscode.SnippetString('script {\n\t$0\n}');
            items.push(scriptItem);
        }

        // Attributes
        if (linePrefix.match(/\w+\s*{\s*$/)) {
            const commonAttributes = ['class', 'id', 'style'];
            commonAttributes.forEach(attr => {
                const item = new vscode.CompletionItem(attr, vscode.CompletionItemKind.Property);
                item.detail = 'Element attribute';
                item.insertText = new vscode.SnippetString(`${attr}: "\${1:value}";`);
                items.push(item);
            });
        }

        return items;
    }

    private isInStyleBlock(document: vscode.TextDocument, position: vscode.Position): boolean {
        const text = document.getText();
        const offset = document.offsetAt(position);
        
        let styleStart = text.lastIndexOf('style {', offset);
        let styleEnd = text.indexOf('}', styleStart);
        
        return styleStart !== -1 && (styleEnd === -1 || styleEnd > offset);
    }

    private isInScriptBlock(document: vscode.TextDocument, position: vscode.Position): boolean {
        const text = document.getText();
        const offset = document.offsetAt(position);
        
        let scriptStart = text.lastIndexOf('script {', offset);
        let scriptEnd = text.indexOf('}', scriptStart);
        
        return scriptStart !== -1 && (scriptEnd === -1 || scriptEnd > offset);
    }
}
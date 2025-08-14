# CHTL User Guide

## Introduction

Welcome to CHTL (C++ Hypertext Language), a powerful templating language that brings compile-time optimization and component-based architecture to web development.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic Syntax](#basic-syntax)
3. [Advanced Features](#advanced-features)
4. [CHTL JavaScript](#chtl-javascript)
5. [Templates and Components](#templates-and-components)
6. [Module System](#module-system)
7. [Best Practices](#best-practices)
8. [Examples](#examples)

## Getting Started

### Installation

1. **Build from Source**
   ```bash
   git clone https://github.com/yourusername/chtl.git
   cd chtl
   mkdir build && cd build
   cmake ..
   make -j4
   sudo make install
   ```

2. **Using Package Manager** (coming soon)
   ```bash
   # Ubuntu/Debian
   sudo apt install chtl
   
   # macOS
   brew install chtl
   ```

### Your First CHTL File

Create a file named `hello.chtl`:

```chtl
// hello.chtl
body {
    h1 {
        text "Hello, CHTL!"
    }
    
    p {
        text "Welcome to modern web development."
    }
}
```

Compile it:
```bash
chtl hello.chtl -o hello.html
```

## Basic Syntax

### HTML Elements

CHTL uses a clean, indentation-based syntax for HTML:

```chtl
body {
    header {
        nav {
            ul {
                li { text "Home" }
                li { text "About" }
                li { text "Contact" }
            }
        }
    }
    
    main {
        h1 {
            text "Welcome"
        }
        
        p {
            text "This is a paragraph."
        }
    }
}
```

### Attributes

Attributes are defined using key-value pairs:

```chtl
div {
    id: "container";
    class: "main-content active";
    data-value: "123";
    
    a {
        href: "https://example.com";
        target: "_blank";
        text "Visit Example"
    }
}
```

### Text Content

Use the `text` keyword for text content:

```chtl
p {
    text "This is plain text"
}

// Multi-line text
div {
    text """
    This is a multi-line
    text content that preserves
    line breaks.
    """
}
```

### Comments

CHTL supports three types of comments:

```chtl
// Single-line comment

/*
 * Multi-line comment
 * Can span multiple lines
 */

-- Generator-recognized comment (preserved in output)
```

## Advanced Features

### Local Style Blocks

Define styles scoped to an element:

```chtl
div {
    style {
        background: #f0f0f0;
        padding: 20px;
        border-radius: 8px;
        
        &:hover {
            background: #e0e0e0;
        }
    }
    
    text "Styled content"
}
```

### Global Style Blocks

Define global CSS styles:

```chtl
style {
    * {
        box-sizing: border-box;
    }
    
    body {
        font-family: Arial, sans-serif;
        margin: 0;
        padding: 0;
    }
    
    .container {
        max-width: 1200px;
        margin: 0 auto;
    }
}
```

### Script Blocks

Embed JavaScript in your CHTL:

```chtl
body {
    button {
        id: "myButton";
        text "Click me"
    }
    
    script {
        document.getElementById('myButton').addEventListener('click', function() {
            alert('Button clicked!');
        });
    }
}
```

## CHTL JavaScript

CHTL extends JavaScript with special syntax for DOM manipulation:

### Selector Syntax

```chtl
script {
    // Select by ID
    {{#myButton}}.style.color = 'red';
    
    // Select by class
    {{.active}}.forEach(el => {
        el.classList.add('highlighted');
    });
    
    // Select by tag
    {{button}}.disabled = false;
}
```

### Event Handling

```chtl
script {
    {{#submitBtn}}.listen({
        click: (e) => {
            e.preventDefault();
            console.log('Form submitted');
        },
        
        mouseenter: () => {
            {{#submitBtn}}.classList.add('hover');
        }
    });
}
```

### Chain Access

```chtl
script {
    // Chain property access
    {{#app}}->innerHTML = '<p>New content</p>';
    
    // Chain method calls
    {{.item}}->classList->add('selected');
}
```

### Animation API

```chtl
script {
    {{#box}}.animate({
        opacity: [0, 1],
        transform: ['translateY(-20px)', 'translateY(0)']
    }, {
        duration: 300,
        easing: 'ease-out'
    });
}
```

## Templates and Components

### Element Templates

Define reusable element templates:

```chtl
[Template] @Element Card {
    div {
        class: "card";
        
        h3 {
            text @title;
        }
        
        p {
            text @content;
        }
    }
}

// Usage
body {
    Card {
        @title: "My Card";
        @content: "This is card content";
    }
}
```

### Style Templates

Create reusable style sets:

```chtl
[Template] @Style PrimaryButton {
    background: #007bff;
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    
    &:hover {
        background: #0056b3;
    }
}

// Usage
button {
    style {
        @Style PrimaryButton;
        font-size: 16px;
    }
    text "Click me"
}
```

### Custom Elements

Define custom element types:

```chtl
[Custom] @Element Modal {
    // Define structure
    div {
        class: "modal-backdrop";
        
        div {
            class: "modal-content";
            
            header {
                h2 { text @title; }
                button {
                    class: "close";
                    text "×"
                }
            }
            
            div {
                class: "modal-body";
                @content;
            }
        }
    }
}

// Usage
Modal {
    @title: "Confirm Action";
    @content: {
        p { text "Are you sure?" }
        button { text "Yes" }
        button { text "No" }
    }
}
```

## Module System

### Importing Modules

```chtl
// Import HTML components
[Import] @Html from "components/header.html" as Header

// Import stylesheets
[Import] @Style from "styles/theme.css" as Theme

// Import CHTL modules
[Import] @Chtl from "ui-components" as UI

// Import specific templates
[Import] [Template] @Element Button from "components.chtl" as MyButton
```

### Using Official Modules

```chtl
// Import official CHTL modules
[Import] @Chtl from "chtl::forms" as Forms
[Import] @Chtl from "chtl::animations" as Anim

body {
    Forms.Input {
        type: "email";
        placeholder: "Enter email";
    }
}
```

### Creating Modules

Structure your module directory:
```
my-module/
├── module.json
├── index.chtl
├── components/
│   ├── button.chtl
│   └── card.chtl
└── styles/
    └── theme.chtl
```

`module.json`:
```json
{
    "name": "my-module",
    "version": "1.0.0",
    "main": "index.chtl",
    "exports": ["Button", "Card", "Theme"]
}
```

## Best Practices

### 1. Component Organization

Keep components small and focused:

```chtl
// Good - Single responsibility
[Template] @Element UserAvatar {
    img {
        src: @avatarUrl;
        alt: @userName;
        class: "avatar";
    }
}

// Avoid - Too many responsibilities
[Template] @Element UserProfile {
    // Too much logic in one component
}
```

### 2. Style Scoping

Prefer local styles for component-specific styling:

```chtl
// Good - Scoped styles
div {
    class: "card";
    style {
        background: white;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
}

// Use global styles for themes
style {
    :root {
        --primary-color: #007bff;
        --secondary-color: #6c757d;
    }
}
```

### 3. Semantic HTML

Use appropriate HTML elements:

```chtl
// Good
nav {
    ul {
        li { a { href: "/home"; text "Home" } }
    }
}

// Avoid
div {
    div {
        div { text "Home" }
    }
}
```

### 4. Performance Tips

- Use templates for repeated elements
- Minimize inline scripts
- Group related styles
- Use the module system for code organization

## Examples

### Complete Page Example

```chtl
// Import components
[Import] @Chtl from "chtl::ui" as UI

// Global styles
style {
    * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }
    
    body {
        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
        line-height: 1.6;
        color: #333;
    }
}

// Page template
[Template] @Element Page {
    body {
        UI.Header {
            @title: "My App";
            @nav: ["Home", "About", "Contact"];
        }
        
        main {
            class: "container";
            @content;
        }
        
        UI.Footer {
            @year: "2024";
            @company: "My Company";
        }
    }
}

// Use the page template
Page {
    @content: {
        h1 { text "Welcome to CHTL" }
        
        section {
            style {
                padding: 2rem 0;
            }
            
            h2 { text "Features" }
            
            div {
                class: "grid";
                
                UI.Card {
                    @title: "Fast";
                    @icon: "⚡";
                    @content: "Compile-time optimization";
                }
                
                UI.Card {
                    @title: "Modern";
                    @icon: "🚀";
                    @content: "Component-based architecture";
                }
                
                UI.Card {
                    @title: "Simple";
                    @icon: "✨";
                    @content: "Clean, readable syntax";
                }
            }
        }
        
        script {
            // Add interactivity
            {{.card}}.listen({
                click: function() {
                    this.animate({
                        transform: ['scale(1)', 'scale(1.05)', 'scale(1)']
                    }, {
                        duration: 200
                    });
                }
            });
        }
    }
}
```

### Interactive Form Example

```chtl
[Template] @Element ContactForm {
    form {
        id: "contactForm";
        
        style {
            max-width: 500px;
            margin: 0 auto;
            
            .form-group {
                margin-bottom: 1rem;
                
                label {
                    display: block;
                    margin-bottom: 0.25rem;
                    font-weight: bold;
                }
                
                input, textarea {
                    width: 100%;
                    padding: 0.5rem;
                    border: 1px solid #ddd;
                    border-radius: 4px;
                }
            }
        }
        
        div {
            class: "form-group";
            label { for: "name"; text "Name" }
            input {
                type: "text";
                id: "name";
                required: true;
            }
        }
        
        div {
            class: "form-group";
            label { for: "email"; text "Email" }
            input {
                type: "email";
                id: "email";
                required: true;
            }
        }
        
        div {
            class: "form-group";
            label { for: "message"; text "Message" }
            textarea {
                id: "message";
                rows: "5";
                required: true;
            }
        }
        
        button {
            type: "submit";
            style {
                @Style PrimaryButton;
                width: 100%;
            }
            text "Send Message"
        }
        
        script {
            {{#contactForm}}.listen({
                submit: (e) => {
                    e.preventDefault();
                    
                    // Get form data
                    const formData = new FormData(e.target);
                    const data = Object.fromEntries(formData);
                    
                    // Show success message
                    {{#contactForm}}->innerHTML = `
                        <div style="text-align: center; padding: 2rem;">
                            <h3>Thank you!</h3>
                            <p>Your message has been sent.</p>
                        </div>
                    `;
                }
            });
        }
    }
}
```

## Troubleshooting

### Common Issues

1. **Compilation Errors**
   - Check syntax - CHTL is indentation-sensitive
   - Ensure all blocks are properly closed
   - Verify attribute syntax (use `:` or `=`)

2. **Styles Not Applied**
   - Local styles only affect their parent element
   - Check CSS selector specificity
   - Ensure style blocks are properly formatted

3. **JavaScript Errors**
   - CHTL JS syntax is only valid within script blocks
   - Regular JavaScript syntax is also supported
   - Check browser console for runtime errors

### Getting Help

- Documentation: https://chtl-lang.org/docs
- GitHub Issues: https://github.com/yourusername/chtl/issues
- Community Forum: https://forum.chtl-lang.org
- Discord: https://discord.gg/chtl

## Next Steps

- Explore the [API Reference](API_Reference.md) for detailed technical information
- Check out the [Examples Repository](https://github.com/chtl-examples) for more code samples
- Join the community to share your projects and get feedback

Happy coding with CHTL!
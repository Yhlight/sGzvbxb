#!/usr/bin/env python3
import sys
from html.parser import HTMLParser

class HTMLFormatter(HTMLParser):
    def __init__(self):
        super().__init__()
        self.indent = 0
        self.output = []
        self.in_pre = False
        
    def handle_starttag(self, tag, attrs):
        if tag in ['br', 'img', 'input', 'meta', 'link']:
            self.output.append('  ' * self.indent + f'<{tag}')
            for attr in attrs:
                self.output[-1] += f' {attr[0]}="{attr[1]}"'
            self.output[-1] += '>'
        else:
            self.output.append('  ' * self.indent + f'<{tag}')
            for attr in attrs:
                self.output[-1] += f' {attr[0]}="{attr[1]}"'
            self.output[-1] += '>'
            if tag not in ['script', 'style']:
                self.indent += 1
            if tag == 'pre':
                self.in_pre = True
                
    def handle_endtag(self, tag):
        if tag not in ['br', 'img', 'input', 'meta', 'link']:
            if tag not in ['script', 'style']:
                self.indent -= 1
            self.output.append('  ' * self.indent + f'</{tag}>')
            if tag == 'pre':
                self.in_pre = False
                
    def handle_data(self, data):
        data = data.strip()
        if data:
            if self.in_pre:
                self.output.append(data)
            else:
                self.output.append('  ' * self.indent + data)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python3 format_html.py <input.html>")
        sys.exit(1)
        
    with open(sys.argv[1], 'r') as f:
        html = f.read()
        
    formatter = HTMLFormatter()
    formatter.feed(html)
    
    for line in formatter.output:
        print(line)
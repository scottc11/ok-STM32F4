#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

// Function to convert a single SVG file
function convertSvgFile(filePath) {
    if (!fs.existsSync(filePath)) {
        console.error(`Error: File '${filePath}' not found`);
        return false;
    }
    
    if (!filePath.toLowerCase().endsWith('.svg')) {
        console.warn(`Warning: '${filePath}' doesn't appear to be an SVG file`);
    }
    
    console.log(`Converting: ${filePath}`);
    
    try {
        // Read the file
        const content = fs.readFileSync(filePath, 'utf8');
        
        // Convert width and height attributes
        const convertedContent = content
            // Convert width="123" or width="123px" to width="1.23mm"
            .replace(/width="(\d+(?:\.\d+)?)(px)?"/g, (match, value) => {
                const mmValue = parseFloat(value) / 100;
                // Format number nicely (remove unnecessary decimals)
                const formatted = mmValue % 1 === 0 ? mmValue.toString() : mmValue.toString();
                return `width="${formatted}mm"`;
            })
            // Convert height="123" or height="123px" to height="1.23mm"
            .replace(/height="(\d+(?:\.\d+)?)(px)?"/g, (match, value) => {
                const mmValue = parseFloat(value) / 100;
                // Format number nicely (remove unnecessary decimals)
                const formatted = mmValue % 1 === 0 ? mmValue.toString() : mmValue.toString();
                return `height="${formatted}mm"`;
            });
        
        // Write the converted content
        fs.writeFileSync(filePath, convertedContent);
        
        console.log(`  ✓ Converted successfully`);
        return true;
        
    } catch (error) {
        console.error(`Error processing ${filePath}:`, error.message);
        return false;
    }
}

// Function to show conversion result
function showConversionResult(filePath) {
    try {
        const content = fs.readFileSync(filePath, 'utf8');
        const svgMatch = content.match(/<svg[^>]*(?:width="[^"]*"|height="[^"]*")[^>]*>/);
        
        if (svgMatch) {
            console.log(`  Result: ${svgMatch[0]}`);
        }
    } catch (error) {
        console.error('Error showing result:', error.message);
    }
}

// Function to find all SVG files recursively
function findSvgFiles(dir) {
    const svgFiles = [];
    
    function searchDir(currentDir) {
        try {
            const items = fs.readdirSync(currentDir);
            
            for (const item of items) {
                const itemPath = path.join(currentDir, item);
                const stat = fs.statSync(itemPath);
                
                if (stat.isDirectory()) {
                    searchDir(itemPath);
                } else if (item.toLowerCase().endsWith('.svg')) {
                    svgFiles.push(itemPath);
                }
            }
        } catch (error) {
            console.error(`Error reading directory ${currentDir}:`, error.message);
        }
    }
    
    searchDir(dir);
    return svgFiles;
}

// Function to process a path (file or directory)
function processPath(inputPath) {
    if (!fs.existsSync(inputPath)) {
        console.error(`Error: Path '${inputPath}' not found`);
        return [];
    }
    
    const stat = fs.statSync(inputPath);
    
    if (stat.isFile()) {
        // It's a file - return it if it's an SVG
        if (inputPath.toLowerCase().endsWith('.svg')) {
            return [inputPath];
        } else {
            console.warn(`Warning: '${inputPath}' is not an SVG file`);
            return [];
        }
    } else if (stat.isDirectory()) {
        // It's a directory - find all SVG files in it
        console.log(`Processing directory: ${inputPath}`);
        return findSvgFiles(inputPath);
    } else {
        console.error(`Error: '${inputPath}' is not a file or directory`);
        return [];
    }
}

// Function to prompt user for confirmation
function promptUser(question) {
    const readline = require('readline');
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });
    
    return new Promise((resolve) => {
        rl.question(question, (answer) => {
            rl.close();
            resolve(answer.toLowerCase().startsWith('y'));
        });
    });
}

// Main function
async function main() {
    console.log('SVG Pixel to Millimeter Converter (Node.js)');
    console.log('=============================================');
    console.log('This script converts width/height from pixels to mm by dividing by 100');
    console.log('Example: width="435" becomes width="4.35mm"');
    console.log('Example: width="1280px" becomes width="12.8mm"');
    console.log('');
    
    const args = process.argv.slice(2);
    
    if (args.length === 0) {
        // No paths specified, find all SVG files in current directory
        console.log('No paths specified. Looking for SVG files in current directory and subdirectories...');
        
        const svgFiles = findSvgFiles('.');
        
        if (svgFiles.length === 0) {
            console.log('No SVG files found in current directory or subdirectories.');
            process.exit(1);
        }
        
        console.log(`Found ${svgFiles.length} SVG file(s):`);
        svgFiles.forEach(file => console.log(`  ${file}`));
        console.log('');
        
        const shouldProceed = await promptUser('Convert all these files? (y/N): ');
        
        if (!shouldProceed) {
            console.log('Cancelled.');
            process.exit(0);
        }
        
        // Convert all found files
        console.log('');
        for (const file of svgFiles) {
            if (convertSvgFile(file)) {
                showConversionResult(file);
            }
        }
        
    } else {
        // Process specified paths (files or directories)
        let allSvgFiles = [];
        
        for (const inputPath of args) {
            const svgFiles = processPath(inputPath);
            allSvgFiles = allSvgFiles.concat(svgFiles);
        }
        
        if (allSvgFiles.length === 0) {
            console.log('No SVG files found to process.');
            process.exit(1);
        }
        
        // Show what we found if processing multiple files
        if (allSvgFiles.length > 1) {
            console.log(`Found ${allSvgFiles.length} SVG file(s) to process:`);
            allSvgFiles.forEach(file => console.log(`  ${file}`));
            console.log('');
        }
        
        // Convert all found files
        for (const file of allSvgFiles) {
            if (convertSvgFile(file)) {
                showConversionResult(file);
            }
        }
    }
    
    console.log('Conversion complete!');
}

// Run the script
if (require.main === module) {
    main().catch(error => {
        console.error('Script error:', error.message);
        process.exit(1);
    });
}

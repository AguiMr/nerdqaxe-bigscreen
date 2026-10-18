const fs = require('fs');
const path = require('path');

const directory = './dist/axe-os';

// The firmware only ever serves the precompressed ".gz" variant (handler_file.cpp
// appends ".gz" to every path), so drop every uncompressed file, including the
// ones in subdirectories like assets/ - they would only waste SPIFFS space.
function removeUncompressed(dir) {
    for (const file of fs.readdirSync(dir)) {
        const filePath = path.join(dir, file);

        if (fs.statSync(filePath).isDirectory()) {
            removeUncompressed(filePath);
        } else if (!file.endsWith('.gz')) {
            fs.unlinkSync(filePath);
            console.log(`Removed file: ${filePath}`);
        }
    }
}

removeUncompressed(directory);

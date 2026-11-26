#!/bin/bash
set -e

echo "Building library with Vite..."
npx vite build -c vite.lib.config.ts

echo "Generating TypeScript declarations..."
npx vue-tsc -p tsconfig.lib.json

echo "Build complete!"
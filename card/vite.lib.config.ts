import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import path from 'path'

export default defineConfig({
  plugins: [vue()],
  build: {
    lib: {
      entry: path.resolve(__dirname, 'src/index.ts'),
      name: 'CardTools',
      fileName: (format) => `card-tools.${format}.js`,
      formats: ['es', 'umd'],
    },
    rollupOptions: {
      external: ['vue'],
      output: {
        globals: {
          vue: 'Vue',
        },
        assetFileNames: (assetInfo) => {
          if (assetInfo.name === 'style.css') return 'style.css'
          // Keep SVG assets with their original names
          if (assetInfo.name?.endsWith('.svg')) return 'assets/[name][extname]'
          // CSS file should be style.css
          if (assetInfo.name?.endsWith('.css')) return 'style.css'
          return `assets/[name]-[hash][extname]`
        },
        entryFileNames: `card-tools.[format].js`,
        chunkFileNames: `assets/[name]-[hash].js`,
      },
    },
    cssCodeSplit: false,
    sourcemap: true,
    emptyOutDir: true,
  },
  publicDir: false,
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
    },
  },
  define: {
    'process.client': 'false',
    'process.server': 'false',
    'import.meta.env.SSR': 'false',
  },
})
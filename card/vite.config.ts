import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import path from 'path'
import { viteSingleFile } from 'vite-plugin-singlefile'

export default defineConfig(() => ({
  plugins: [vue(), viteSingleFile()],
  base: process.env.BASE_URL || '/',
  resolve: {
    alias: {
      '@': path.resolve(import.meta.dirname, './src'),
      '~/': path.resolve(import.meta.dirname, './src/'),
      '~~/': path.resolve(import.meta.dirname, './src/'),
      '#app': path.resolve(import.meta.dirname, './src/'), // compat Nuxt
    },
  },
  define: {
    'process.client': 'false',
    'process.server': 'false',
    'import.meta.env.SSR': 'false',
  },
  server: {
    port: 3000,
  },
}))
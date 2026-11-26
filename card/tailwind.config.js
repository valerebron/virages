/** @type {import('tailwindcss').Config} */

export default {
  content: [
    './index.html',
    './src/**/*.{vue,js,ts,jsx,tsx}',
  ],
  theme: {
    extend: {
      colors: {
        primary: {
          DEFAULT: '#fdec50ff',
          light: '#fdec50ff',
          dark: '#fdec50ff',
        },
        esyellow: '#fdec50ff'
      },
      boxShadow: {
        '2xl': '0 25px 50px -12px rgba(0, 0, 0, 0.5)',
        '2xl-custom': '0 0 60px 50px rgba(0, 0, 0, 0.25)'
      },
      fontSize: {
        xxs: '0.625rem'
      },
      screens: {
        'xsm': '340px'
      },
      transitionDuration: {
        2000: '2000ms'
      }
    }
  },
  plugins: [
    function ({ addUtilities }) {
      addUtilities({
        '.screen-centered': {
          position: 'fixed',
          inset: '0',
          height: '100dvh',
          width: '100dvw',
          display: 'grid',
          'place-items': 'center'
        },
        '.full-screen': {
          height: '100dvh',
          width: '100dvw'
        },
        '.size-100vmin': {
          width: '100vmin',
          height: '100vmin'
        },
        '.debug': {
          position: 'absolute',
          'z-index': '1000',
          bottom: '16px',
          right: '25%',
          background: '#9CA3AF',
          'border-radius': '16px',
          padding: '16px'
        },
        '.debug > *': {
          display: 'block'
        }
      })
    }
  ],
}
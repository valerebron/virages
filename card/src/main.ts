import { createApp } from 'vue'
import touchInterfacePlugin from './plugins/touchInterface'
import './style.css'
import App from './App.vue'

createApp(App)
  .use(touchInterfacePlugin)
  .mount('#app')
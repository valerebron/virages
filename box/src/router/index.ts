import { createRouter, createWebHistory } from 'vue-router'
import App from '../App.vue'

export default createRouter({
  history: createWebHistory(),
  routes: [
    { path: '/', component: App },
    { path: '/card/:id', component: App, props: true },
  ],
})
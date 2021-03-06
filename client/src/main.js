import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import '@/assets/style.scss'
import axios from 'axios'
import VueAxios from 'vue-axios'
 
Vue.use(VueAxios, axios)
Vue.config.productionTip = false
Vue.config.devtools = true
window.Vue = Vue;
new Vue({
  router,
  store,
  render: h => h(App)
}).$mount('#app')

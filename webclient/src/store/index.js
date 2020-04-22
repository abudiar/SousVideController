import Vue from 'vue'
import Vuex from 'vuex'
import axios from 'axios'
import VueAxios from 'vue-axios'
 
Vue.use(VueAxios, axios)
Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    temp: 166.5,
    scale: 'C',
    target: 166.5,
    state: 'preheat',
    isConnected: false
  },
  getters: {
    status(state) {
      if (!state.isConnected)
        return 'nc';
      switch(state.state) {
        case 'Off':
          return 'stopped';
        default:
          return 'running';
      }
    },
    tempText(state, getters) {
      if (getters.status == 'nc') 
        return '';
      switch(state.scale) {
        case 'C':
          return state.temp;
        case 'F':
          return Math.round(((state.temp * 9) / 5 + 32) * 100) / 100;
      }
    },
    stateText(state, getters) {
      if (getters.status == 'nc') 
        return '';
      return state.state;
    }
  },
  mutations: {
    updateData(state, payload){
      for(let key in payload){
        console.log('Updating', key, 'with:', payload[key]);
        state[key] = payload[key];
      }
      // if (payload.temp)
      //   state.temp = payload.temp;
      // if (payload.scale)
      //   state.scale = payload.scale;
      // if (payload.target)
      //   state.target = payload.target;
      // if (payload.state)
      //   state.state = payload.state;
      // if (payload.isConnected)
      //   state.isConnected = payload.isConnected;
      // if (payload.kp)
      //   state.kp = payload.kp;
      // if (payload.ki)
      //   state.ki = payload.ki;
      // if (payload.kd)
      //   state.kd = payload.kd;
    }
  },
  actions: {
    fetch({commit}) {
      Vue.axios.get('/fetch')
        .then((response) => {
          console.log(response);
          commit({
            type: 'updateData',
            isConnected: true,
            ...response.data
          })
        })
        .catch((err) => {
          commit({
            type: 'updateData',
            isConnected: false
          })
          console.log(err)
        });
    }
  },
  modules: {
  }
})

import Vue from 'vue'
import Vuex from 'vuex'
import axios from 'axios'
import VueAxios from 'vue-axios'
import { config } from '@/helpers/config';
axios.defaults.baseURL = config.$url;
 
Vue.use(VueAxios, axios)
Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    temp: 0,
    scale: 'C',
    target: 0,
    state: '',
    isConnected: false,
    firstRun: true,
    kp: 0,
    ki: 0,
    kd: 0
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
    },
    targetText(state, getters) {
      if (getters.status == 'nc') 
        return '';
      switch(state.scale) {
        case 'C':
          return state.target;
        case 'F':
          return Math.round(((state.target * 9) / 5 + 32) * 100) / 100;
      }
    },
  },
  mutations: {
    updateData(state, payload){
      for(let key in payload){
        if (state[key] != payload[key]) {
          console.log('Updating', key, 'with:', payload[key]);
          state[key] = payload[key];
        }
      }
    },
  },
  actions: {
    getData({/* state, */commit}) {
      Vue.axios.get('/data')
        .then((response) => {
          console.log('getData', response);
          commit({
            ...response.data,
            isConnected: true,
            firstRun: false,
            type: 'updateData',
          })
        })
        .catch((err) => {
          commit({
            firstRun: true,
            isConnected: false,
            type: 'updateData',
          })
          console.log('getData', err)
        });
    },
    postData: function({state,commit}, payload) {
      const oldState = {...state}
      commit({
        ...payload,
        type: 'updateData'
      })
      Vue.axios.post('/data?data=' + JSON.stringify(payload))
        .then((response) => {
          console.log('postData', response);
          commit({
            ...response.data,
            isConnected: true,
            firstRun: false,
            type: 'updateData',
          })
        })
        .catch((err) => {
          commit({
            ...oldState,
            isConnected: false,
            firstRun: true,
            type: 'updateData',
          })
          console.log('postData', err)
        });
    },
    postCommand({state, commit}, payload) {
      const oldState = {...state}
      commit({
        ...payload,
        type: 'updateData',
      })
      Vue.axios.post('/command?command=' + payload.command)
        .then((response) => {
          console.log('postCommand', response);
        })
        .catch((err) => {
          commit({
            ...oldState,
            type: 'updateData',
            isConnected: false,
            firstRun: true,
          })
          console.log('postCommand', err)
        });
    },
  },
  modules: {
  }
})

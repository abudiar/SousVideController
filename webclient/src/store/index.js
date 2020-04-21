import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    temp: 166.5,
    scale: 'C',
    target: 166,
    state: 'preheat',
  },
  getters: {
    status(state) {
      // if (state.state)
      //   return 'nc';
      switch(state.state) {
        case 'off':
          return 'stopped';
        default:
          return 'running';
      }
    },
    tempText(state, getters) {
      if (getters.status == 'nc') 
        return 'X';
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
  },
  actions: {
  },
  modules: {
  }
})

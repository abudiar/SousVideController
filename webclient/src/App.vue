<template>
  <div id="app">
    <Home class="home" @showSettings="showSettings = true"/>
    <Settings class="settings" @hideSettings="showSettings = false"
      :class="{'showSettings': showSettings}"/>
  </div>
</template>

<script>
import Home from "./views/Home"
import Settings from "./views/Settings"
export default {
  components: {
    Home,
    Settings
  },
  data() {
    return {
      showSettings: false
    }
  },
  created() {
    setInterval(() => {
      this.$store.dispatch('fetch')
    }, 5000)
  },
}
</script>

<style lang="scss">
#app {
  // @import url('https://fonts.googleapis.com/css2?family=Montserrat:wght@300&display=swap');
  font-family: BlinkMacSystemFont, -apple-system, "Segoe UI", "Roboto", "Oxygen", "Ubuntu", "Cantarell", "Fira Sans", "Droid Sans", "Helvetica Neue", "Helvetica", "Arial", sans-serif;
  font-weight: 300;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  background: #FFFAFA;
  display: grid;
  grid-template-columns: 1fr;
  grid-template-rows: 1fr auto;
  overflow: hidden;
  height: 100vh;
  .home {
    grid-area: 1/1/3/1;
  }
  .settings {
    grid-area: 2/1/3/1;
    &.showSettings {
      max-height: 100vh;
    }
  }
}
</style>

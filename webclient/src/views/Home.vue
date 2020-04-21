<template>
  <div class="home">
    <div class="page">
      <TempCircle class="temp-circle"/>
    </div>
    <StartButton class="start-button"/>
  </div>
</template>

<script>
// @ is an alias to /src
import TempCircle from '@/components/TempCircle.vue'
import StartButton from '@/components/StartButton.vue'

export default {
  name: 'Home',
  components: {
    TempCircle,
    StartButton
  }
}
</script>

<style scoped lang="scss">
.home {
  display: grid;
  grid-template-columns: 1fr auto 1fr;
  grid-template-rows: auto 1fr auto;
  grid-template-areas: 
      ". page ."
      ". . ."
      ". start-button .";
  min-height: 100vh;
  width: 100vw;
  max-width: 100vw;
  .page {
    grid-area: page;
    display: flex;
    flex-direction: column;
    border-radius: 1em;
    padding: 4em 2em;
    margin: 1em;
    background: white;
    min-width: 36em;
    -webkit-box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
    -moz-box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
    box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
  }
  .start-button {
    grid-area: start-button;
    padding: 2em;
  }
}
</style>
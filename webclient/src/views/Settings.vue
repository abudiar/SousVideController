<template>
  <div class="settings transition">
    <div class="page-container">
      <div class="page">
        <div class="topBar">
          <a class="back" >
            <span>Close</span>
          </a>
          <h2 class="title">Settings</h2>
        </div>
        <h4>Tunings</h4>
        <BInput :input="'String(target)'" 
          :left="'Kp'" 
          @input="$store.dispatch('target',$event)" 
          :inputType="'number'" />
        <BInput :input="'String(target)'" 
          :left="'Ki'" 
          @input="$store.dispatch('target',$event)" 
          :inputType="'number'" />
        <BInput :input="'String(target)'" 
          :left="'Kd'" 
          @input="$store.dispatch('target',$event)" 
          :inputType="'number'" />

      </div>
    </div>
  </div>
</template>

<script>
import BInput from '@/components/BInput.vue'

export default {
  name: 'Settings',
  components: {
    BInput
  },
}
</script>


<style scoped lang="scss">
.settings {
  width: 100vw;
  max-width: 100vw;
  box-sizing: border-box;
  margin-bottom:-1em;
  max-height: 0;
    // display: grid;
    // grid-template-columns: auto 1fr auto;
    // grid-template-rows: 1fr;
    // grid-template-areas: 
    //     ". page .";
  .page-container {
      // grid-area: page;
    padding: 1em;
    padding-bottom:0;
    width: 100%;
    box-sizing: border-box;
    display: flex;
    justify-content: center;
    .page {
      box-sizing: border-box;
      display: flex;
      flex-direction: column;
      border-radius: 1em;
      padding: 2em;
      background: white;
      min-width: 25em;
      max-width: 36em;
      width: 100%;
      -webkit-box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      -moz-box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      .topBar {
        display: grid;
        grid-template-columns: 1fr 1fr 1fr;
        grid-template-rows: 1fr;
        grid-template-areas: 
            "back title .";
        .back {
          grid-area: back;
          display: flex;
          flex-direction: row;
          // justify-content: center;
          text-align: left;
          padding: 0 2em;

          span {
            display: flex;
            flex-direction: column;
            justify-content: center;
            // background: 
          }
        }
        .title {
          grid-area: title;
        }
      }
    }
  }
}
</style>
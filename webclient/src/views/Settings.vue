<template>
  <div class="settings transition">
    <div class="page-container">
      <div class="page">
        <div class="topBar">
          <a class="back" @click="$emit('hideSettings')">
            <span>Close</span>
          </a>
          <h2 class="title">Settings</h2>
        </div>
        <hr>
        <br>
        <h4>Tunings</h4>
        <BInput :input="String(kp)" 
          :left="'Kp'" 
          @input="$store.dispatch({
            type: 'postData',
            kp: $event
          })" 
          :inputType="'number'" />
        <BInput :input="String(ki)" 
          :left="'Ki'" 
          @input="$store.dispatch({
            type: 'postData',
            ki: $event
          })" 
          :inputType="'number'" />
        <BInput :input="String(kd)" 
          :left="'Kd'" 
          @input="$store.dispatch({
            type: 'postData',
            kd: $event
          })" 
          :inputType="'number'" />
        <hr>
        <br>
        <h4>Log</h4>
        <Chart/>
      </div>
    </div>
  </div>
</template>

<script>
import BInput from '@/components/BInput.vue'
import Chart from '@/components/Chart.vue'

export default {
  name: 'Settings',
  components: {
    BInput,
    Chart
  },
  computed: {
    kp () {
      return this.$store.state.kp;
    },
    kd () {
      return this.$store.state.kd;
    },
    ki () {
      return this.$store.state.ki;
    }
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
    overflow-y: auto;
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
      // display: flex;
      // flex-direction: column;
      border-radius: 1em;
      padding: 2em;
      padding-bottom: 3em;
      background: white;
      min-width: 25em;
      max-width: 36em;
      width: 100%;
      -webkit-box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      -moz-box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      hr {
        width: 90%;
        opacity: 0.3;
      }
      h4 {
        text-align: left;
        padding-left: 2em;
      }
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
            cursor: pointer;
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
<template>
  <div class="home">
    <div class="page-container">
      <div class="page">
        <TempCircle class="temp-circle" @showSettings="$emit('showSettings')" />
        <br />
        <br />
        <div class="form">
          <NumInput
            :input="Number(targetText)"
            :decimals="1"
            :left="'Temperature'"
            :right="scale"
            @input="$store.dispatch({
            type: 'postData',
            target: $event
          })"
          />
          <div class="container-switch">
            <div class="switch-back switch-button"></div>
            <div class="switch-button">
              <span class="active" :style="activeSwitchStyle"></span>
              <button
                class="switch-button-case left"
                :class="leftButtonClass"
                @click="$store.dispatch({
                  type: 'postData',
                  scale: 'C'
                })"
              >Celcius</button>
              <button
                class="switch-button-case right"
                :class="rightButtonClass"
                @click="$store.dispatch({
                  type: 'postData',
                  scale: 'F'
                })"
              >Fahrenheit</button>
            </div>
          </div>
        </div>
      </div>
    </div>
    <StartButton class="start-button" />
  </div>
</template>

<script>
// @ is an alias to /src
import TempCircle from "@/components/TempCircle.vue";
import StartButton from "@/components/StartButton.vue";
import NumInput from "@/components/NumInput.vue";

export default {
  name: "Home",
  components: {
    TempCircle,
    StartButton,
    NumInput
  },
  data() {
    return {
      // scale: 'C',
      // target: ''
    };
  },
  computed: {
    targetText() {
      return this.$store.getters.targetText;
    },
    scale() {
      return this.$store.state.scale;
    },
    leftButtonClass() {
      return {
        "active-case": this.scale == "C",
        "not-active": this.scale != "C"
      };
    },
    rightButtonClass() {
      return {
        "active-case": this.scale == "F",
        "not-active": this.scale != "F"
      };
    },
    activeSwitchStyle() {
      return {
        left: this.scale == "C" ? "0%" : "47%",
        backgroundPosition: this.scale == "C" ? "0%" : "100%"
      };
    }
  },
  watch: {
    scale() {
      // inputTarget.value = validateNum(inputTarget, 1);
    },
    target() {
      console.log(this.target);
    }
  }
};
</script>

<style scoped lang="scss">
.home {
  background: url("https://images.unsplash.com/photo-1558030006-450675393462?ixlib=rb-1.2.1&ixid=eyJhcHBfaWQiOjEyMDd9&w=1000&q=80") no-repeat;
  background-size: cover;
  display: grid;
  grid-template-columns: auto 1fr auto;
  grid-template-rows: auto 1fr auto;
  grid-template-areas:
    "page page page"
    ". . ."
    ". start-button .";
  min-height: 100vh;
  width: 100vw;
  max-width: 100vw;
  overflow-y: auto;
  .page-container {
    grid-area: page;
    padding: 1em;
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
      width: 100%;
      max-width: 36em;
      -webkit-box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      -moz-box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.15);
      .form {
        text-align: left;
        .input {
          display: grid;
          grid-template-columns: auto 1fr auto;
          grid-template-areas: "left middle right";
          p {
            margin: 0;
          }
          .left {
            padding-bottom: 0.1em;
            grid-area: left;
            display: flex;
            flex-direction: column;
            justify-content: center;
            padding: 0 1em;
          }
          .middle {
            grid-area: middle;
            display: flex;
            flex-direction: column;
            justify-content: center;
            .inputText {
              padding-top: 0.1em;
              text-align: right;
              font-size: 2em;
              background: none;
              border: none;
            }
          }
          .right {
            padding-bottom: 0.1em;
            display: flex;
            flex-direction: column;
            justify-content: center;
            grid-area: right;
            padding: 0 1em;
            font-size: 1.5em;
          }
        }
      }
    }
  }
  .start-button {
    grid-area: start-button;
    padding: 2em;
  }
}
</style>
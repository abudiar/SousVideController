<template>
    <div class="container-switch">
      <div class=" switch-button" :class="{'focused': isFocused}">
      </div>
      <div class="switch-button input">
        <div class="padl transition" :class="{'focused': isFocused}">
        </div>
        <div v-if="right" class="padr transition" :class="{'focused': isFocused}">
        </div>
        <div class="middle">
          <input class="inputText" 
            align="right" 
            type="number" 
            :value="input.toFixed(decimals)" 
            :step="10^(-decimals)"
            @change="$emit('input', $event.target.value)"
            @focus="isFocused = true"
            @blur="isFocused = false"/>
        </div>
        <div class="left">
          <p class="transition" :class="{'focused': isFocused}">{{left}}</p>
        </div>
        <div v-if="right" class="right">
          <p class="transition" :class="{'focused': isFocused}">°{{right}}</p>
        </div>
      </div>
    </div>
</template>

<script>
// @ is an alias to /src

export default {
  name: 'NumInput',
  props: {
    right: String,
    input: Number,
    left: String,
    decimals: Number
  },
  data() {
    return {
      isFocused: false
    }
  }
}
</script>

<style scoped lang="scss">
  .input {
    display: grid;
    grid-template-columns: auto auto auto auto auto;
    grid-template-areas: 
        "padl left middle right padr";
    padding: 0 2em;
    p {
      color: rgba(0, 0, 0, 0.418);
      margin: 0;
    }
    .left {
      padding-bottom: .1em;
      grid-area: left;
      display: flex;
      flex-direction: column;
      justify-content: center;
      // padding: 0 1em;
    }
    .middle {
      grid-area: middle;
      display: flex;
      flex-direction: column;
      justify-content: center;
      .inputText {
        width: 100%;
        padding-top: .1em;
        text-align: right; 
        font-size: 2em;
        background: none;
        border: none;
        &:focus {
          outline: none;
        }
      }
    }
    .right {
      padding-bottom: .1em;
      display: flex;
      flex-direction: column;
      justify-content: center;
      grid-area: right;
      // padding: 0 1em;
      font-size: 1.5em;
    }
    .focused {
      letter-spacing: 0.1em;
      font-weight: 350;
      color: black;
    }
    .padl {
      grid-area: padl;
      width: 1em;
    }
    .padr {
      grid-area: padr;
      width: 1em;
    }
  }
  
  .switch-back.focused {
    // opacity: .5;
  }
</style>
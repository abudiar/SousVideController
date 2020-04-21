<template>
    <div class="container">
        <div class="temp-circle-wrap transition" :class="statusClass">
            <div class="temp-circle transition" :class="statusClass">
                <div class="top">
                    <p>{{stateText}}</p>
                </div>
                <div class="middle">
                    <div class="text">
                        <p class="temp">{{tempText}}</p>
                        <p class="sup">{{supText}}</p>
                    </div>
                    <p class="sub">{{subText}}</p>
                </div>
                <div class="bottom">
                    <svg class="settings transition" xmlns="http://www.w3.org/2000/svg" height="512pt" viewBox="0 0 512 512" width="512pt">
                        <path d="m499.953125 197.703125-39.351563-8.554687c-3.421874-10.476563-7.660156-20.695313-12.664062-30.539063l21.785156-33.886719c3.890625-6.054687 3.035156-14.003906-2.050781-19.089844l-61.304687-61.304687c-5.085938-5.085937-13.035157-5.941406-19.089844-2.050781l-33.886719 21.785156c-9.84375-5.003906-20.0625-9.242188-30.539063-12.664062l-8.554687-39.351563c-1.527344-7.03125-7.753906-12.046875-14.949219-12.046875h-86.695312c-7.195313 0-13.421875 5.015625-14.949219 12.046875l-8.554687 39.351563c-10.476563 3.421874-20.695313 7.660156-30.539063 12.664062l-33.886719-21.785156c-6.054687-3.890625-14.003906-3.035156-19.089844 2.050781l-61.304687 61.304687c-5.085937 5.085938-5.941406 13.035157-2.050781 19.089844l21.785156 33.886719c-5.003906 9.84375-9.242188 20.0625-12.664062 30.539063l-39.351563 8.554687c-7.03125 1.53125-12.046875 7.753906-12.046875 14.949219v86.695312c0 7.195313 5.015625 13.417969 12.046875 14.949219l39.351563 8.554687c3.421874 10.476563 7.660156 20.695313 12.664062 30.539063l-21.785156 33.886719c-3.890625 6.054687-3.035156 14.003906 2.050781 19.089844l61.304687 61.304687c5.085938 5.085937 13.035157 5.941406 19.089844 2.050781l33.886719-21.785156c9.84375 5.003906 20.0625 9.242188 30.539063 12.664062l8.554687 39.351563c1.527344 7.03125 7.753906 12.046875 14.949219 12.046875h86.695312c7.195313 0 13.421875-5.015625 14.949219-12.046875l8.554687-39.351563c10.476563-3.421874 20.695313-7.660156 30.539063-12.664062l33.886719 21.785156c6.054687 3.890625 14.003906 3.039063 19.089844-2.050781l61.304687-61.304687c5.085937-5.085938 5.941406-13.035157 2.050781-19.089844l-21.785156-33.886719c5.003906-9.84375 9.242188-20.0625 12.664062-30.539063l39.351563-8.554687c7.03125-1.53125 12.046875-7.753906 12.046875-14.949219v-86.695312c0-7.195313-5.015625-13.417969-12.046875-14.949219zm-152.160156 58.296875c0 50.613281-41.179688 91.792969-91.792969 91.792969s-91.792969-41.179688-91.792969-91.792969 41.179688-91.792969 91.792969-91.792969 91.792969 41.179688 91.792969 91.792969zm0 0"/>
                    </svg>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
export default {
  name: 'TempCircle',
  props: {
    msg: String
  },
  data() {
      return {
          colors: {
              padding: {

              }
          }
      }
  },
  computed: {
      status() {
          return this.$store.getters.status;
      },
      scale() {
          return this.$store.state.scale;
      },
      statusClass() {
          const statusClass = {};
          if (this.status)
            statusClass[this.status] = true;  
          return statusClass;
      },
      subText() {
          if (this.status == 'nc') 
            return '';
        return 'Current Temperature';
      },
      supText() {
          if (this.status == 'nc') 
            return '';
          return '°' + this.scale;
      },
      tempText() {
          return this.$store.getters.tempText;
      },
      stateText() {
          return this.$store.getters.stateText;
      }
  }
}
</script>

<style scoped lang="scss">
.transition {
    -webkit-transition: all 0.4s;
    -o-transition: all 0.4s;
    -moz-transition: all 0.4s;
    transition: all 0.4s;
}
.container {
    display: grid;
    grid-template-columns: 1fr auto 1fr;
    grid-template-rows: 1fr auto 1fr;
    grid-template-areas: 
        ". . ."
        ". temp-circle ."
        ". . .";
    .temp-circle-wrap {
        grid-area: temp-circle;
        padding: .7em;
        margin: 0;
        border-radius: 50%;
        background: linear-gradient(to top, #ffc1c2, #ffd8c4);
        &.nc {
            background: linear-gradient(to top, #d4d4d4, #e4e4e4);
        }
        &.stopped {
            background: linear-gradient(to top, #c1cfff, #c4ffff);
        }
        &.preheat {
            background: linear-gradient(to top, #ffc1f2, #ffc4c4);
        }
        .temp-circle {
            width: 9.4em;
            height: 9.4em;
            font-size: 1.4rem;
            // letter-spacing: .2rem;
            // text-transform: uppercase;
            border-radius: 50%;
            background: white;
            color: #333333;
            display: flex;
            justify-content: center;
            background: linear-gradient(to top, #ff5e62, #ff9966);
            padding: 0em;
            display: grid;
            grid-template-columns: 1fr;
            grid-template-rows: 1fr 1fr 1fr;
            grid-template-areas: 
                "top"
                "middle"
                "bottom";
            &.nc {
                background: linear-gradient(to top, #b3b3b3, #cecece);
            }
            &.stopped {
                background: linear-gradient(to top, #5e69ff, #66fff2);
            }
            &.preheat {
                background: linear-gradient(to top, #ff5ea1, #ff6666);
            }
            p {
                margin: 0;
                padding: 0;
                font-size: .8em;
                font-weight: light;
                color: white;
            }
            .top {
                grid-area: top;
                display: flex;
                flex-direction: column;
                justify-content: center;
                letter-spacing: .2rem;
                text-transform: uppercase;
                margin-top: 1.5em;
            }
            .middle {
                grid-area: middle;
                display: flex;
                flex-direction: column;
                justify-content: center;
                display: grid;
                grid-template-columns: 1fr;
                grid-template-rows: auto;
                grid-template-areas: 
                    "text"
                    "sub";
                .text {
                    display: grid;
                    grid-template-columns: auto auto;
                    grid-template-rows: auto;
                    grid-template-areas: 
                        "temp sup";
                    margin-bottom: -.6em;
                    .temp {
                        grid-area: temp;
                        font-size: 2.8em;
                        text-align: right;
                        font-weight: 400;
                    }
                    .sup {
                        grid-area: sup;
                        font-size: 1.5em;
                        text-align: left;
                        padding-top: 0.4em;
                        color: rgba(255, 255, 255, 0.6);
                    }
                }
                .sub {
                    grid-area: sub;
                    color: rgba(255, 255, 255, 0.6);
                }
            }
            .bottom {
                grid-area: bottom;
                display: flex;
                flex-direction: column;
                justify-content: flex-end;
                align-items: center;
                margin-bottom: .3em;
                .settings {
                    cursor: pointer;
                    fill: rgba(255, 255, 255, 0.5);
                    height: .9em;
                    width: .9em;
                    &:hover {
                        fill: white;
                    }
                }
            }
        }
    }
}
</style>

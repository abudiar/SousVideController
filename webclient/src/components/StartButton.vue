<template>
    <div class="start-button">
        <div class="wrap-btn transition" :class="btnClass">
            <button class="btn transition">
                {{ btnText }}
            </button>
        </div>
    </div>
</template>

<script>
export default {
  name: 'StartButton',
  props: {
    msg: String
  },
  computed: {
      btnText() {
          const status = this.$store.getters.status;
          if (status == 'stopped')
            return 'Start';
          else if (status == 'nc')
            return 'X';
          else
            return 'Stop';
      },
      btnClass() {
          const status = this.$store.getters.status;
          if (status == 'stopped')
            return {'stopped': true};
          else if (status == 'nc')
            return {};
          else
            return {'started': true};
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
.start-button {
    display: grid;
    grid-template-columns: 1fr auto 1fr;
    grid-template-rows: 1fr auto 1fr;
    grid-template-areas: 
        ". . ."
        ". btn ."
        ". . .";
    .wrap-btn {
        grid-area: btn;
        padding: .3em;
        margin: 0;
        border-radius: 50%;
        background: linear-gradient( 45deg, rgb(224, 224, 224) 0%, rgb(180, 180, 180) 100% );
        -webkit-box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0.3);
        -moz-box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0.3);
        box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0.3);
        .btn {
            width: 4.7em;
            height: 4.7em;
            font-size: 1.4rem;
            font-weight: 300;
            letter-spacing: .2rem;
            text-transform: uppercase;
            border-radius: 50%;
            background: white;
            border: .05em solid white;
            color: #333333;
            display: flex;
            justify-content: center;
            box-shadow: inset 0 3px 5px rgba(0, 0, 0, 0.125);
            &:focus {
                outline:0;
            }
        }
        &.stopped {
            background: linear-gradient( 45deg, rgb(147, 245, 249) 0%, rgb(201, 255, 204) 100% );
            &:hover {
                margin: .3em;
                padding: 0;
                -webkit-box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0);
                -moz-box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0);
                box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0);
                .btn {
                    cursor: pointer;
                    border: .2em solid white;
                    letter-spacing: .3rem;
                    color: white;
                    background: linear-gradient( 225deg, rgb(147, 245, 249) 0%, rgb(201, 255, 204) 100% );
                }
            }
        }
        &.started {
            background: linear-gradient( 45deg, rgb(249, 147, 172) 0%, rgb(255, 232, 201) 100% );
            &:hover {
                margin: .3em;
                padding: 0;
                -webkit-box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0);
                -moz-box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0);
                box-shadow: 5px 5px 40px -10px rgba(0, 0, 0, 0);
                .btn {
                    cursor: pointer;
                    border: .2em solid white;
                    letter-spacing: .3rem;
                    color: white;
                    background: linear-gradient( 225deg, rgb(249, 147, 172) 0%, rgb(255, 232, 201) 100% );
                }
            }
        }
    }
}
</style>

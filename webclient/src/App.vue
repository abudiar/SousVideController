<template>
  <div id="app">
    <router-view/>
  </div>
</template>

<style lang="scss">
#app {
  // @font-face { 
  //   font-family: "Montserrat-Light";
  //   src: url("assets/Montserrat-Light.ttf"); 
  // }
  // @font-face { 
  //   font-family: "Montserrat-Medium";
  //   src: url("assets/Montserrat-Medium.ttf"); 
  // }
  // @font-face { 
  //   font-family: "Montserrat-Regular";
  //   src: url("assets/Montserrat-Regular.ttf"); 
  // }
  @import url('https://fonts.googleapis.com/css2?family=Montserrat:wght@300&display=swap');
  font-family: 'Montserrat', sans-serif;
  font-weight: 300;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  background: #FFFAFA;
}
</style>

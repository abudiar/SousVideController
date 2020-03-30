(function ($) {
  "use strict";

  var body = document.querySelector("body");
  var divCard = document.querySelector(".card");

  var switchBtnRight = document.querySelector(".switch-button-case.right");
  var switchBtnLeft = document.querySelector(".switch-button-case.left");
  var activeSwitch = document.querySelector(".switch-button .active");

  var containerBtn = document.querySelector(".container-btn");
  var wrapBtnStart = document.querySelector(".container-btn .start");
  var wrapBtnStop = document.querySelector(".container-btn .stop");
  var btnStart = document.querySelector(".btn.start");
  var btnStop = document.querySelector(".btn.stop");

  var btnSettings = document.querySelector(".symbol .settings");
  var btnAutotune = document.querySelector(".btn-autotune");
  var btnSave = document.querySelector(".settings-btn");
  var btnDiscard = document.querySelector(".btn-discard");

  var textCurTemp = document.querySelector(".curTemp");
  var textSymbol = document.querySelector(".symbol-end-input");
  var textState = document.querySelector(".state");

  var inputTarget = document.querySelector(".target");
  var inputKp = document.querySelector(".kp");
  var inputKi = document.querySelector(".ki");
  var inputKd = document.querySelector(".kd");

  var lastTemp = "24.0";
  var pageNum = 1;
  var fetchDataObj = {};

  function sendCommand(cmd) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        // console.log(this.responseText);
        if (pageNum == 0)
          switchOnline(true);
      }
    };
    xhttp.open("GET", "/command?command=" + cmd, true);
    xhttp.onerror = function () {
      console.log("** An error occurred during the transaction");
      switchOnline(false);
    };
    xhttp.send();
  }

  function sendData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        // console.log(this.responseText);
        if (pageNum == 0)
          switchOnline(true);
      }
    };
    var dataObj = {};

    dataObj["pageNum"] = pageNum;

    if (pageNum == 0) {
      if (switchBtnLeft.classList.contains("active-case")) {
        dataObj["scale"] = "C";
        dataObj["target"] = inputTarget.value;
        // textCurTemp.innerHTML = inputTarget.value + "<sup>°C</sup>";
      } else {
        dataObj["scale"] = "F";
        dataObj["target"] = convertToTemp("C", inputTarget.value);
        // textCurTemp.innerHTML = inputTarget.value + "<sup>°F</sup>";
      }

    } else {
      dataObj["kp"] = inputKp.value;
      dataObj["ki"] = inputKi.value;
      dataObj["kd"] = inputKd.value;
    }
    console.log(JSON.stringify(dataObj));
    xhttp.open("GET", "/update?data=" + JSON.stringify(dataObj), true);
    xhttp.onerror = function () {
      console.log("** An error occurred during the transaction");
      if (pageNum == 0)
        switchOnline(false);
    };
    xhttp.send();
  }

  function getData(justTemp = false) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        // console.log(this.responseText);
        updatePage(this.responseText, justTemp);
        if (pageNum == 0) switchOnline(true);
      }
    };
    xhttp.open("GET", "/fetch", true);
    xhttp.onerror = function () {
      // console.log("** An error occurred during the transaction");
      switchOnline(false);
    };
    xhttp.send();
  }

  function sendAndRecieveData(cmd = "") {
    if (cmd == "")
      sendData();
    else
      sendCommand(cmd);
    setTimeout(() => {
      getData();
    }, 300);
  }

  function updatePage(data, justTemp = false) {
    fetchDataObj = JSON.parse(data);
    if (pageNum == 0) {
      textState.innerHTML = fetchDataObj["state"];
      if (fetchDataObj["scale"] == "F") {
        switchScale(fetchDataObj["scale"]);
        // textCurTemp.innerHTML = `BLEH<sup>°${fetchDataObj["scale"]}</sup>`;
        textCurTemp.innerHTML = `${validateNum(convertToTemp("F", fetchDataObj["temp"]), 1, true)}<sup>°${fetchDataObj["scale"]}</sup>`;
        inputTarget.value = validateNum(convertToTemp("F", fetchDataObj["target"]), 1, true);
      } else {
        switchScale(fetchDataObj["scale"]);
        // textCurTemp.innerHTML = `BLUH<sup>°${fetchDataObj["scale"]}</sup>`;
        textCurTemp.innerHTML = `${validateNum(fetchDataObj["temp"], 1, true)}<sup>°${fetchDataObj["scale"]}</sup>`;
        inputTarget.value = validateNum(fetchDataObj["target"], 1, true);
      }
      if (fetchDataObj["state"] == "Off") {
        switchRunningBtn(false);
      }
      else {
        switchRunningBtn(true);
      }
      if (!justTemp)
        console.log(JSON.stringify(fetchDataObj));
    } else {
      inputKp.value = String(fetchDataObj["kp"]);
      inputKi.value = String(fetchDataObj["ki"]);
      inputKd.value = String(fetchDataObj["kd"]);
      inputKp.value = validateNum(inputKp, 0);
      inputKi.value = validateNum(inputKi, 2);
      inputKd.value = validateNum(inputKd, 2);
      tuningsChanged();
    }
  }

  function tuningsChanged() {
    if ("kp" in fetchDataObj && "ki" in fetchDataObj && "kd" in fetchDataObj) {
      console.log(Number(fetchDataObj["kp"]) + " " + Number(inputKp.value) + " " +
        Number(fetchDataObj["ki"]) + " " + Number(inputKi.value) + " " +
        Number(fetchDataObj["kd"]) + " " + Number(inputKd.value));
      if (
        Number(fetchDataObj["kp"]) != Number(inputKp.value) ||
        Number(fetchDataObj["ki"]) != Number(inputKi.value) ||
        Number(fetchDataObj["kd"]) != Number(inputKd.value)
      ) {
        hideDiscard(false);
        hideSave(false);
      }
      else { hideDiscard(true); hideSave(true); }
    } else { hideDiscard(false); hideSave(false); } // for testing
  }

  function revertTunings() {
    if ("kp" in fetchDataObj && "ki" in fetchDataObj && "kd" in fetchDataObj) {
      getData();
    } else {
      // for testing
      inputKp.value = 850;
      inputKi.value = 0.5;
      inputKd.value = 0.1;
    }
    hideDiscard(true);
  }

  function hideDiscard(bool) {
    if (bool) btnDiscard.classList.add("hidden");
    else btnDiscard.classList.remove("hidden");
  }

  function hideSave(bool) {
    if (bool) btnSave.classList.add("hidden");
    else btnSave.classList.remove("hidden");
  }

  function switchOnline(bool) {
    if (bool) {
      textCurTemp.classList.remove("dimmed");
      textState.innerHTML = fetchDataObj["state"];
    } else {
      textCurTemp.innerHTML =
        '<span class="glyphicon glyphicon-remove"></span>';
      textCurTemp.classList.add("dimmed");
      textCurTemp.style.setProperty("Margin-right", "3px")
      textState.innerHTML = "404";
    }
  }

  function convertToTemp(scale, temp) {
    if (scale == "F") {
      return Math.round(((temp * 9) / 5 + 32) * 100) / 100;
    } else {
      return Math.round((((temp - 32) * 5) / 9) * 100) / 100;
    }
  }

  function switchScale(scale) {
    if (scale == "C") switchCelcius();
    else switchFahrenheit();
  }

  function switchCelcius() {
    if (!switchBtnLeft.classList.contains("active-case")) {
      switchBtnRight.classList.remove("active-case");
      switchBtnLeft.classList.add("active-case");
      switchBtnRight.classList.add("not-active");
      switchBtnLeft.classList.remove("not-active");
      activeSwitch.style.left = "0%";
      activeSwitch.style.backgroundPosition = "0%";
      inputTarget.value = convertToTemp("C", inputTarget.value);
      textSymbol.innerHTML = "°C";
      inputTarget.min = 24;
      inputTarget.max = 99;
      inputTarget.value = validateNum(inputTarget, 1);
    }
  }

  function switchFahrenheit() {
    if (!switchBtnRight.classList.contains("active-case")) {
      switchBtnRight.classList.add("active-case");
      switchBtnLeft.classList.remove("active-case");
      switchBtnRight.classList.remove("not-active");
      switchBtnLeft.classList.add("not-active");
      activeSwitch.style.left = "47%";
      activeSwitch.style.backgroundPosition = "100%";
      inputTarget.value = convertToTemp("F", inputTarget.value);
      textSymbol.innerHTML = "°F";
      inputTarget.min = 74;
      inputTarget.max = 210;
      inputTarget.value = validateNum(inputTarget, 1);
    }
  }

  function switchRunningBtn(started) {
    if (started) {
      wrapBtnStart.classList.add("hidden");
      wrapBtnStop.classList.remove("hidden");
    } else {
      wrapBtnStart.classList.remove("hidden");
      wrapBtnStop.classList.add("hidden");
    }
  }
  function countDecimals(valueString) {
    var value = parseFloat(valueString);
    return valueString.split(".")[1].length || 0;
  }
  function getZeros(num) {
    var zeros = "";
    for (var i = 0; i < num; i++) zeros += "0";
    return zeros;
  }

  function validateNum(target, decimalPlaces, isNotElement = false) {
    var min = target.min;
    var max = target.max;
    var value = String(target.value);
    if (isNotElement)
      value = String(target);
    var result = "";

    if (!isNaN(parseFloat(value))) {
      if (parseFloat(value) < parseFloat(min)) {
        value = min.toString();
      } else if (parseFloat(value) > parseFloat(max)) {
        value = max.toString();
      }

      if (decimalPlaces <= 0 && value.toString().includes(".")) {
        result = value.toString().split(".")[0];
      }
      else if (decimalPlaces <= 0)
        result = value.toString();
      else if (!value.toString().includes(".")) {
        result = `${value.toString()}.${getZeros(decimalPlaces)}`;
      }
      else {
        var cntDecs = countDecimals(value);
        if (cntDecs < decimalPlaces) {
          result = value.toString() + getZeros(decimalPlaces - cntDecs);
        } else if (cntDecs > decimalPlaces) {
          result = value.toString().substring(0, value.toString().split(".")[0].length + 1 + decimalPlaces);
        } else {
          result = value.toString();
        }
      }
    } else {
      return lastTemp.toString();
    }

    lastTemp = value.toString();
    return result;
  }

  function isElementInViewport(el) {
    // Special bonus for those using jQuery
    if (typeof jQuery === "function" && el instanceof jQuery) {
      el = el[0];
    }

    var rect = el.getBoundingClientRect();

    return (
      rect.top >= 0 &&
      rect.left >= 0 &&
      rect.bottom <=
      (window.innerHeight ||
        document.documentElement.clientHeight) /* or $(window).height() */ &&
      rect.right <=
      (window.innerWidth ||
        document.documentElement.clientWidth) /* or $(window).width() */
    );
  }

  function areElementsClashing(top, bottom) {
    // Special bonus for those using jQuery
    if (typeof jQuery === "function" && top instanceof jQuery) {
      top = top[0];
    }
    // Special bonus for those using jQuery
    if (typeof jQuery === "function" && bottom instanceof jQuery) {
      bottom = bottom[0];
    }
    var rectTop = top.getBoundingClientRect();
    var rectBot = bottom.getBoundingClientRect();
    return rectTop.bottom > rectBot.top;
  }

  function checkIfFit() {
    if (pageNum == 0) {
      // console.log("checkIfFit");
      if (body.scrollTop == 0 && isElementInViewport(containerBtn)) {
        containerBtn.style.left = "0";
        containerBtn.style.bottom = "0";
        containerBtn.style.position = "absolute";
      }
      if (body.scrollTop == 0 && areElementsClashing(divCard, containerBtn)) {
        containerBtn.style.left = "auto";
        containerBtn.style.bottom = "auto";
        containerBtn.style.position = "relative";
      }
    }
  }

  if (inputTarget != null) {
    pageNum = 0;
    inputTarget.addEventListener(
      "change",
      function () {
        inputTarget.value = validateNum(inputTarget, 1);
        sendAndRecieveData();
      },
      false
    );
  }

  if (inputKp != null)
    inputKp.addEventListener(
      "change",
      function () {
        inputKp.value = validateNum(inputKp, 0);
        tuningsChanged();
      },
      false
    );

  if (inputKi != null)
    inputKi.addEventListener(
      "change",
      function () {
        inputKi.value = validateNum(inputKi, 2);
        tuningsChanged();
      },
      false
    );

  if (inputKd != null)
    inputKd.addEventListener(
      "change",
      function () {
        inputKd.value = validateNum(inputKd, 2);
        tuningsChanged();
      },
      false
    );

  if (btnStart != null)
    btnStart.addEventListener(
      "click",
      function () {
        sendAndRecieveData("start");
      },
      false
    );

  if (btnStop != null)
    btnStop.addEventListener(
      "click",
      function () {
        sendAndRecieveData("stop");
      },
      false
    );

  if (btnSave != null)
    btnSave.addEventListener(
      "click",
      function () {
        sendAndRecieveData();
      },
      false
    );

  if (btnAutotune != null)
    btnAutotune.addEventListener(
      "click",
      function () {
        sendAndRecieveData("autotune");
      },
      false
    );

  if (btnDiscard != null)
    btnDiscard.addEventListener(
      "click",
      function () {
        revertTunings();
      },
      false
    );

  if (switchBtnLeft != null)
    switchBtnLeft.addEventListener(
      "click",
      function () {
        switchCelcius();
        sendAndRecieveData();
      },
      false
    );

  if (switchBtnRight != null)
    switchBtnRight.addEventListener(
      "click",
      function () {
        switchFahrenheit();
        sendAndRecieveData();
      },
      false
    );

  window.addEventListener(
    "resize",
    function () {
      checkIfFit();
    },
    false
  );

  checkIfFit();
  getData();
  setInterval(function () {
    // Call a function repetatively with 2 Second interval
    if (pageNum == 0)
      getData(true);
  }, 5000); //2000mSeconds update rate

  /*==================================================================
    [ Validate ]*/
  var input = $(".validate-input .input");

  $(".validate-form").on("submit", function () {
    var check = true;

    for (var i = 0; i < input.length; i++) {
      if (validate(input[i]) == false) {
        showValidate(input[i]);
        check = false;
      }
    }

    return check;
  });

  $(".validate-form .input").each(function () {
    $(this).focus(function () {
      hideValidate(this);
    });
  });

  function validate(input) {
    if ($(input).attr("type") == "email" || $(input).attr("name") == "email") {
      if (
        $(input)
          .val()
          .trim()
          .match(
            /^([a-zA-Z0-9_\-\.]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([a-zA-Z0-9\-]+\.)+))([a-zA-Z]{1,5}|[0-9]{1,3})(\]?)$/
          ) == null
      ) {
        return false;
      }
    } else {
      if (
        $(input)
          .val()
          .trim() == ""
      ) {
        return false;
      }
    }
  }

  function showValidate(input) {
    var thisAlert = $(input).parent();

    $(thisAlert).addClass("alert-validate");
  }

  function hideValidate(input) {
    var thisAlert = $(input).parent();

    $(thisAlert).removeClass("alert-validate");
  }
})(jQuery);

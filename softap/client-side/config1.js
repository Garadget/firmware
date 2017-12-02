function f_postRequest(s_url, a_postData, a_handlers) {
    var o_request = new XMLHttpRequest();
    o_request.open('POST', s_baseUrl + s_url, true);
    o_request.timeout = 4000;
    o_request.setRequestHeader('Content-Type', 'multipart/form-data');
    o_request.onreadystatechange = function() {
        if (o_request.readyState == 4 && a_handlers) {
          window.n_busy--;
          if (o_request.status == 200) {
              if (a_handlers.success)
                a_handlers.success(JSON.parse(o_request.responseText));
          }
          else if (a_handlers.error)
            a_handlers.error(o_request.status, o_request.responseText);
          if (a_handlers.regardless) a_handlers.regardless();
        }
    };
    window.n_busy = (window.n_busy || 0) + 1;
    o_request.send(JSON.stringify(a_postData));
}

function f_getRequest(s_url, a_handlers) {
  // debug
/*  if (String(window.location).indexOf('192.168.0.1') == -1) {
    window.n_busy = (window.n_busy || 0) + 1;
    window.setTimeout(function(){
      window.n_busy--;
      var a_tests = {
        'get-config':  {
          sys:"0.6.2", id:"330038000451353530353431",ver:"1.13",mac:"00:00:00:00:00:00",
          ssid:"TP-LINK",rdt:1000,mtt:10000,rlt:300,rlp:1000,srt:15,
          nme:"Garage",mqon:255,mqip:"255.255.255.255",mqpt:65535,mqto:65535,mqtt:2
        },
        'scan-ap': {scans:[
          {ssid:"Huawei_HG532e_BF6A7",rssi:-63,sec:4194310,ch:11,mdr:270000},
          {ssid:"PS4-60632F95A700",rssi:-67,sec:0,ch:6,mdr:144400},
          {ssid:"Bloomberg",rssi:-62,sec:4194310,ch:8,mdr:270000},
          {ssid:"TP-LINK",rssi:-65,sec:4194308,ch:3,mdr:300000},
          {ssid:"UKrtelecom_WPe7Eg",rssi:-48,sec:4194308,ch:6,mdr:135000},
          {ssid:"EZCastmsy-4C3F0370",rssi:-63,sec:4194308,ch:11,mdr:150000}
        ]},
        'public-key': {
          "b":"30819F300D06092A864886F70D010101050003818D0030818902818100B4B572EA55C4C3F0D55D3AFC31B7F52A9F41BD7595DBB7D98FDA60E19A584C7E9E7ABE384C1E1EDDCB0B530A35E5FDBF4F44F4A0D98E03385A1498B4CDB9FDF30DBB7A95A8387A482CC519C69366AF40EF35CFC28DB5941C33E3D30320C01FBAE8E4E9409B360FF7A7F88E7D082D2D88F445408CCC4D70366665A8F0382334990203010001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000","r":0}
      };
      a_handlers.success(a_tests[s_url]);
      if (a_handlers.regardless)
        a_handlers.regardless();
    }, 1000);
    return;
  }*/

  var o_request = new XMLHttpRequest();
  o_request.open('GET', s_baseUrl + s_url, true);
  o_request.timeout = 8000;
  o_request.onreadystatechange = function() {
    if (o_request.readyState == 4 && a_handlers) {
      window.n_busy--;
      if (o_request.status == 200) {
        if (a_handlers.success)
          a_handlers.success(JSON.parse(o_request.responseText));
      }
      else if (a_handlers.error)
        a_handlers.error(o_request.status, o_request.responseText);
      if (a_handlers.regardless)
        a_handlers.regardless();
    }
  };
  window.n_busy = (window.n_busy || 0) + 1;
  o_request.send();
}

function f_getById(s_id) {
  var e_elem = document.getElementById(s_id);
  return e_elem ? e_elem : document.forms[0].elements[s_id];
}

function f_inputValue(s_name, s_value) {
  var e_input = document.forms[0].elements[s_name];
  if (!e_input) return false;
  var s_oldValue, n_option;

  // select boxes
  if (e_input.options) {
    s_oldValue = e_input.options[e_input.selectedIndex].value;
    if (arguments.length > 1) {
      for (n_option = 0; n_option < e_input.options.length; n_option++)
        if (e_input.options[n_option].value == s_value) {
          e_input.selectedIndex = n_option;
          break;
        }
    }
  }
  // check boxes
  else if (e_input.type == 'checkbox') {
    s_oldValue = e_input.checked ? e_input.value : false;
    e_input.checked = s_value;
  }
  // radio buttons
  else if (e_input.length) {
    var n_selectedIndex = -1;
    for (n_option = 0; n_option < e_input.length; n_option++) {
      if (arguments.length > 1 && e_input[n_option].value == s_value)
        n_selectedIndex = n_option;
      if (e_input[n_option].checked)
        s_oldValue = e_input[n_option].value;
    }
    if (n_selectedIndex >= 0)
      e_input[n_selectedIndex].checked = true;
  }

  // text and password
  if (e_input.value != undefined) {
    s_oldValue = e_input.value;
    if (arguments.length > 1)
      e_input.value = s_value;
  }
  return s_oldValue;
}

function f_populateSelect(s_name, a_options, s_default, s_value) {
  var e_select = document.forms[0].elements[s_name];
  e_select.options.length = 0;
  var s_optionValue, b_valueFound = false;
  for (var n_option = 0; n_option < a_options.length; n_option += 2) {
    s_optionValue = a_options[n_option + 1];
    e_select.options[e_select.options.length] = new Option(a_options[n_option], s_optionValue);
    if (arguments.length > 3 && s_optionValue == s_value) {
      e_select.selectedIndex = n_option / 2;
      b_valueFound = true;
    }
  }
  if (!b_valueFound && arguments.length > 2)
    f_inputValue(s_name, s_default);
}

function f_signalStrength(n_db) {
  var s_strength;
  if (n_db <= -85)
    s_strength = 'poor';
  else if (n_db <= -60)
    s_strength = 'good';
  else
    s_strength = 'excellent';
  return n_db + 'dB (' + s_strength + ')';
}

function f_copyId() {
  window.prompt('Copy to clipboard: Ctrl + C, Enter', f_inputValue('id'));
}

function f_togglePass(s_id) {
  var e_input = f_getById(s_id);
  e_input.type = e_input.type == 'password' ? 'text' : 'password';
}

function f_submitValid(b_valid) {
  var e_button = f_getById('btn-submit');
  if (b_valid)
    e_button.className = e_button.className.replace(/\s*button-passive\s*/, '');
  else if (!e_button.className.match('button-passive'))
    e_button.className += ' button-passive';
  return b_valid;
}

function f_message(s_message, b_error) {
  var e_box = f_getById(b_error ? 'error-box' : 'message-box');
  if (!s_message) {
    e_box.style.display = 'none';
    return;
  }
  e_box.innerHTML = '<li>' + s_message + '</li>';
  e_box.style.display = 'block';
  setTimeout(function() { f_message('', b_error); }, 5000);
}

function f_validate(s_name, f_callback) {
  document.forms[0].elements[s_name].onkeyup = f_callback;
}

// ------------------------------------------------------------------------------------------
// validator object

var c_validator = function() {

  this.a_flags = {
    ssid: false,
    pass: false,
    mqip: false,
    mqpt: false,
    nme: false,
    pkey: false
  };

  this.f_isValid = function() {
    // console.log(this.a_flags);
    if (window.b_busy)
      return false;
    for (var s_flag in this.a_flags)
      if (!this.a_flags[s_flag])
        return false;
    return true;
  };

  this.f_setSubmit = function() {
    f_getById('btn-submit').className = this.f_isValid() ? '' : 'button-passive';
  };

  this.f_setValid = function(s_prop, s_value) {
    this.a_flags[s_prop] = s_value;
    this.f_setSubmit();
  };

  this.f_checkValid = function(s_params) {
    this.s_focus = '';
    var a_params = s_params.split(',');
    for (var n_param in a_params) {
      var s_param = a_params[n_param];
      this.a_flags[s_param] = this['f_val' + s_param]();
    }
    this.f_setSubmit();
  };

  this.f_focus = function(s_id) {
    if (!this.s_focus)
      this.s_focus = s_id;
  };

  this.a_tests = {};

  // individual tests - manual ssid
  this.f_valssid = function() {
    this.a_tests[0] = [];
    var n_value = f_inputValue('ap');
    if (n_value != -1) {
      f_boxManual(false);
      return true;
    }

    f_boxManual(true);
    var s_ssid = f_inputValue('ssid-value');
    if (s_ssid.length)
      return true;
    this.a_tests[0].push('SSID is Requred for Manual Configuration');
    this.f_focus('ssid-value');
    return false;
  };

  // individual tests - password
  this.f_valpass = function() {
    this.a_tests[1] = [];
    var n_value = f_inputValue('ap');
    if ((n_value < 0 && !parseInt(f_inputValue('ap-sec'))) ||
      (n_value >= 0 && !a_networks[n_value].sec)) {
      f_boxPass(false);
      return true;
    }

    f_boxPass(true);
    var s_value = f_inputValue('pass');
    if (s_value.length > 7)
      return true;
    this.a_tests[1].push(s_value.length ?
      'WiFi Password is Too Short' :
      'Password is Required for Secured Network'
    );
    this.f_focus('ap-pass');
    return false;
  };

  // individual tests - MQTT IP
  this.f_valmqip = function() {
    this.a_tests[2] = [];
    if (!(f_inputValue('mqtt') & 2)) {
      f_boxSwitch(false, 'box-mqtt');
      return true;
    }

    f_boxSwitch(true, 'box-mqtt');
    var s_value = f_inputValue('mqip');
    if (!s_value.length) {
      this.a_tests[2].push('MQTT Server IP is Required');
      this.f_focus('mqip');
      return false;
    }

    var a_value = s_value.split('.');
    for (var n_byte = 0; n_byte < 4; n_byte++) {
      if (a_value.length != 4 || a_value[n_byte] == '' || isNaN(a_value[n_byte]) || a_value[n_byte] < 0 || a_value[n_byte] > 0xFF) {
        this.a_tests[2].push('Invalid Format of MQTT Server IP');
        this.f_focus('mqip');
        return false;
      }
    }
    return true;
  };

  this.f_valmqpt = function() {
    this.a_tests[3] = [];
    if (!(f_inputValue('mqtt') & 2))
      return true;

    var n_value = f_inputValue('mqpt');
    if (!n_value.length) {
      this.a_tests[3].push('MQTT Server Port is Required');
      this.f_focus('mqpt');
      return false;
    }
    if (isNaN(n_value) || n_value <= 0 || n_value > 0xFFFF) {
      this.a_tests[3].push('Invalid Value of MQTT Server Port');
      this.f_focus('mqpt');
      return false;
    }
    return true;
  };

  this.f_valnme = function() {
    this.a_tests[4] = [];
    if (!(f_inputValue('mqtt') & 2))
      return true;

    var s_value = f_inputValue('nme');
    if (!s_value.length) {
      this.a_tests[4].push('MQTT Device Topic is Required');
      this.f_focus('nme');
      return false;
    }
    if (!s_value.match(/^[\w\d\_]{1,31}$/)) {
      this.a_tests[4].push('Invalid Value of MQTT Device Topic');
      this.f_focus('nme');
      return false;
    }
    return true;
  };

  this.f_getErrors = function() {
    var a_errors = [];
    for (var s_test in this.a_tests)
      if (this.a_tests[s_test].length)
        for (var s_error in this.a_tests[s_test])
          a_errors.push(this.a_tests[s_test][s_error]);
    if (this.s_focus)
      f_getById(this.s_focus).focus();
    return a_errors;
  };

};

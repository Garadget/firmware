function f_postRequest(s_url, a_postData, a_handlers) {
    var o_request = new XMLHttpRequest();
    o_request.open('POST', s_baseUrl + s_url, true);
    o_request.timeout = 4000;
    o_request.setRequestHeader('Content-Type', 'multipart/form-data');
    o_request.send(JSON.stringify(a_postData));
    o_request.onreadystatechange = function() {
        if (o_request.readyState == 4 && a_handlers) {
          if (o_request.status == 200) {
              if (a_handlers.success)
                a_handlers.success(JSON.parse(o_request.responseText));
          }
          else if (a_handlers.error)
            a_handlers.error(o_request.status, o_request.responseText);
          if (a_handlers.regardless) a_handlers.regardless();
        }
    };
};

function f_getRequest(s_url, a_handlers) {
  // debug
  if (String(window.location).indexOf('192.168.0.1') == -1) {
    if (s_url == 'config')
      a_handlers.success({"sys":"0.6.2", "id":"330038000451353530353431","ver":"1.13","mac":"00:00:00:00:00:00","ssid":"TP-LINK","rdt":1000,"mtt":10000,"rlt":300,"rlp":1000,"srr":3,"srt":15,"nme":"Garage","mqon":255,"mqip":"255.255.255.255","mqpt":65535,"mqto":65535});
    if (s_url == 'scan-ap')
      a_handlers.success({"scans":[{"ssid":"Huawei_HG532e_BF6A7","rssi":-63,"sec":4194310,"ch":11,"mdr":270000},{"ssid":"PS4-60632F95A700","rssi":-67,"sec":0,"ch":6,"mdr":144400},{"ssid":"Bloomberg","rssi":-62,"sec":4194310,"ch":8,"mdr":270000},{"ssid":"TP-LINK","rssi":-65,"sec":4194308,"ch":3,"mdr":300000},{"ssid":"UKrtelecom_WPe7Eg","rssi":-48,"sec":4194308,"ch":6,"mdr":135000},{"ssid":"EZCastmsy-4C3F0370","rssi":-63,"sec":4194308,"ch":11,"mdr":150000}]});
    if (s_url == 'public-key')
      a_handlers.success({"b":"30819F300D06092A864886F70D010101050003818D0030818902818100B4B572EA55C4C3F0D55D3AFC31B7F52A9F41BD7595DBB7D98FDA60E19A584C7E9E7ABE384C1E1EDDCB0B530A35E5FDBF4F44F4A0D98E03385A1498B4CDB9FDF30DBB7A95A8387A482CC519C69366AF40EF35CFC28DB5941C33E3D30320C01FBAE8E4E9409B360FF7A7F88E7D082D2D88F445408CCC4D70366665A8F0382334990203010001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000","r":0});
    if (a_handlers.regardless)
      a_handlers.regardless();
    return;
  }

  var o_request = new XMLHttpRequest();
  o_request.open('GET', s_baseUrl + s_url, true);
  o_request.timeout = 8000;
  o_request.send();
  o_request.onreadystatechange = function() {
    if (o_request.readyState == 4 && a_handlers) {
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
};

/*

var configure = function(a) {
    a.preventDefault();
    var b = get_selected_network();
    var c = document.getElementById('password').value;
    if (!b) {
        window.alert('Please select a network!');
        return false;
    }
    var d = {
        idx: 0,
        ssid: b.ssid,
        pwd: rsa.encrypt(c),
        sec: b.sec,
        ch: b.ch
    };
    connectButton.innerHTML = 'Sending credentials...';
    disableButtons();
    console.log('Sending credentials: ' + JSON.stringify(d));
    postRequest(base_url + 'configure-ap', d, configure_callback);
};

var configure_callback = {
    success: function(a) {
        console.log('Credentials received.');
        connectButton.innerHTML = 'Credentials received...';
        postRequest(base_url + 'connect-ap', {
            idx: 0
        }, connect_callback);
    },
    error: function(a, b) {
        console.log('Configure error: ' + a);
        window.alert('The configuration command failed, check that you are still well connected to the device\\'
            s WiFi hotspot and retry.
            ');connectButton.innerHTML='
            Retry ';enableButtons();}};var connect_callback={success:function(a){console.log('
            Attempting to connect to the cloud.
            ');connectButton.innerHTML='
            Attempting to connect...';window.alert('
            Your device should now start flashing green and attempt to connect to the cloud.This usually takes about 20 seconds, after which it will begin slowly blinking cyan.\\n\\ n\\ nIf this process fails because you entered the wrong password, the device will flash green indefinitely.In this
            case, hold the setup button
            for 6 seconds until the device starts blinking blue again.Then reconnect to the WiFi hotspot it generates and reload this page to
            try again.
            ');},error:function(a,b){console.log('
            Connect error:
                '+a);window.alert('
                The connect command failed, check that you are still well connected to the device\\ 's WiFi hotspot and retry.');
        connectButton.innerHTML = 'Retry';
        enableButtons();
    }
};
*/

function f_getById(s_id) {
  return document.getElementById(s_id);
}

function f_inputValue(s_name, s_value) {
  var e_input = document.forms[0].elements[s_name];
  var s_oldValue, n_option;
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
  else if (e_input.type == 'checkbox') {
    s_oldValue = e_input.checked ? e_input.value : false;
    e_input.checked = s_value;
  }
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

function f_validate(s_name, f_callback) {
  document.forms[0].elements[s_name].onkeydown = f_callback;
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

function f_togglePass() {
  var e_input = f_getById('ap-pass');
  e_input.type = e_input.type == 'password' ? 'text' : 'password';
}

function f_boxManual(b_visibility) {
  var e_boxStyle = f_getById('box-ssid').style;
  if (b_visibility) {
    e_boxStyle.display = 'block';
    var e_input = f_getById('ssid-value');
    e_input.value = '';
    e_input.focus();
    return;
  }
  e_boxStyle.display = 'none';
}

function f_boxPass(b_visibility) {
  var e_boxStyle = f_getById('box-pass').style;
  if (b_visibility) {
    e_boxStyle.display = 'block';
    var e_input = f_getById('ap-pass');
    e_input.value = '';
    e_input.focus();
    return;
  }
  e_boxStyle.display = 'none';
}

function f_apChange(n_network) {
  if (n_network < 0) {
    f_boxPass(true);
    f_boxManual(true);
    return;
  }
  var a_network = window.a_networks[n_network];
  f_boxPass(a_network.sec);
  f_boxManual(false);
  f_inputValue('ssid-value', a_network.ssid);
}

function f_message(s_message, b_error) {
  var e_box = f_getById(b_error ? 'error-box' : 'message-box');
  if (!s_message) {
    e_box.style.display = 'none';
    return;
  }
  e_box.innerHTML = s_message;
  e_box.style.display = 'block';
  setTimeout(function() { f_message('', b_error)}, 5000);
}

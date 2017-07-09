var s_baseUrl = 'http://192.168.0.1/';

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
      a_handlers.success({"id":"330038000451353530353431","ver":"1.13","mac":"00:00:00:00:00:00","ssid":"TP-LINK","rdt":1000,"mtt":10000,"rlt":300,"rlp":1000,"srr":3,"srt":15,"nme":"Garage","mqon":255,"mqip":"255.255.255.255","mqpt":65535,"mqto":65535});
    if (s_url == 'scan-ap')
      a_handlers.success({"scans":[{"ssid":"Huawei_HG532e_BF6A7","rssi":-63,"sec":4194310,"ch":11,"mdr":270000},{"ssid":"PS4-60632F95A700","rssi":-67,"sec":4194308,"ch":6,"mdr":144400},{"ssid":"Bloomberg","rssi":-62,"sec":4194310,"ch":8,"mdr":270000},{"ssid":"TP-LINK","rssi":-65,"sec":4194308,"ch":3,"mdr":300000},{"ssid":"UKrtelecom_WPe7Eg","rssi":-48,"sec":4194308,"ch":6,"mdr":135000},{"ssid":"EZCastmsy-4C3F0370","rssi":-63,"sec":4194308,"ch":11,"mdr":150000}]});
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









var network_list;
var public_key;
var rsa = new RSAKey();
var scanButton = document.getElementById('scan-button');
var connectButton = document.getElementById('connect-button');
var copyButton = document.getElementById('copy-button');
var showButton = document.getElementById('show-button');
var deviceID = document.getElementById('device-id');
var connectForm = document.getElementById('connect-form');

var public_key_callback = {
    success: function(a) {
        console.log('Public key: ' + a.b);
        public_key = a.b;
        rsa.setPublic(public_key.substring(58, 58 + 256), public_key.substring(318, 318 + 6));
    },
    error: function(a, b) {
        console.log(a);
        window.alert('There was a problem fetching important information from your device. Please verify your connection, then reload this page.');
    }
};
var device_id_callback = {
    success: function(a) {
        var b = a.id;
        deviceID.value = b;
    },
    error: function(a, b) {
        console.log(a);
        var c = 'COMMUNICATION_ERROR';
        deviceID.value = c;
    }
};
var scan = function() {
    console.log('Scanning...!');
    disableButtons();
    scanButton.innerHTML = 'Scanning...';
    connectButton.innerHTML = 'Connect';
    document.getElementById('connect-div').style.display = 'none';
    document.getElementById('networks-div').style.display = 'none';
    getRequest(base_url + 'scan-ap', scan_callback);
};
var scan_callback = {
    success: function(a) {
        network_list = a.scans;
        console.log('I found:');
        var b = document.getElementById('networks-div');
        b.innerHTML = '';
        if (network_list.length > 0)
            for (var c = 0; c < network_list.length; c++) {
                ssid = network_list[c].ssid;
                console.log(network_list[c]);
                add_wifi_option(b, ssid);
                document.getElementById('connect-div').style.display = 'block';
            } else b.innerHTML = '<p class=\\'
        scanning - error\\ '>No networks found.</p>';
    },
    error: function(a) {
        console.log('Scanning error:' + a);
        document.getElementById('networks-div').innerHTML = '<p class=\\'
        scanning - error\\ '>Scanning error.</p>';
    },
    regardless: function() {
        scanButton.innerHTML = 'Re-Scan';
        enableButtons();
        document.getElementById('networks-div').style.display = 'block';
    }
};

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

var disableButtons = function() {
    connectButton.disabled = true;
    scanButton.disabled = true;
};

var enableButtons = function() {
    connectButton.disabled = false;
    scanButton.disabled = false;
};

var add_wifi_option = function(a, b) {
    var c = document.createElement('INPUT');
    c.type = 'radio';
    c.value = b;
    c.id = b;
    c.name = 'ssid';
    c.className = 'radio';
    var d = document.createElement('DIV');
    d.className = 'radio-div';
    d.appendChild(c);
    var e = document.createElement('label');
    e.htmlFor = b;
    e.innerHTML = b;
    d.appendChild(e);
    a.appendChild(d);
};
var get_selected_network = function() {
    for (var a = 0; a < network_list.length; a++) {
        ssid = network_list[a].ssid;
        if (document.getElementById(ssid).checked) return network_list[a];
    }
};
var copy = function() {
    window.prompt('Copy to clipboard: Ctrl + C, Enter', deviceID.value);
};
var toggleShow = function() {
    var a = document.getElementById('password');
    inputType = a.type;
    if (inputType === 'password') {
        showButton.innerHTML = 'Hide';
        a.type = 'text';
    } else {
        showButton.innerHTML = 'Show';
        a.type = 'password';
    }
};

if (scanButton.addEventListener) {
    copyButton.addEventListener('click', copy);
    showButton.addEventListener('click', toggleShow);
    scanButton.addEventListener('click', scan);
    connectForm.addEventListener('submit', configure);
} else if (scanButton.attachEvent) {
    copyButton.attachEvent('onclick', copy);
    showButton.attachEvent('onclick', toggleShow);
    scanButton.attachEvent('onclick', scan);
    connectForm.attachEvent('onsubmit', configure);
}
getRequest(base_url + 'device-id', device_id_callback);
getRequest(base_url + 'public-key', public_key_callback);

function f_rescanWifi() {

}

function f_formSubmit() {

}
*/

/*
function f_copyDeviceId() {
}
*/

//var f_addEventListener;
//if (window.addEventListener)
//f_addEventListener =

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
  alert('toggle');
}

function f_apScan() {
  var s_html = '', e_apList = f_getById('aplist');
  e_apList.innerHTML = '<li>Loading...</li>';

  f_getRequest('scan-ap', {
    success: function(a_response) {
      var a_securityTypes = {
        0: 'Not Secured',
        1: 'WEP pre-shared key',
        0x8001: 'Open WEP',
        0x00200002: 'WPA with TKIP',
        0x00200004: 'WPA with AES',
        0x00400004: 'WPA2 with TKIP',
        0x00400002: 'WPA2 with AES',
        0x00400006: 'WPA2 AES & TKIP'
      };
      var n_network, a_networks = a_response.scans;
      for (n_network = 0; n_network < a_networks.length; n_network++)
        a_networks[n_network].value = n_network;
      a_networks.sort(function(a, b) {
        return b.rssi - a.rssi;
      });

      for (n_network = 0; n_network < a_networks.length; n_network++) {
        var a_network = a_networks[n_network];
        s_html += '<li><input type="radio" name="ap" value="' + a_network.value
          + '"' + (window.s_ssid == a_network.ssid ? 'checked' : '') + ' /><div>'
          + a_network.ssid + '<span>' + (a_securityTypes[a_network.sec]
          ? a_securityTypes[a_network.sec] + ', ' : '') + f_signalStrength(a_network.rssi)
          + '</span></div></li>';
      }
      if (!n_network)
        s_html += '<li>No networks found</li>';
    },
    error: function () {
      s_html += '<li style="color:red">Error Scanning Networks</li>';
    },
    regardless: function() {
      s_html += '<li><input type="radio" name="ap" value="-" /><div>Enter Manually<span>Use For Hidden Networks</span></div></li>';
      e_apList.innerHTML = s_html;
      f_apChange();
    }
  });
}

function f_apChange() {
  var s_value = f_inputValue('ap');
  f_getById('box-pass').style.display = 'block';
  console.log(s_value);
}

/*if (scanButton.addEventListener) {
    copyButton.addEventListener('click', copy);
    showButton.addEventListener('click', toggleShow);
    scanButton.addEventListener('click', scan);
    connectForm.addEventListener('submit', configure);
} else if (scanButton.attachEvent) {
    copyButton.attachEvent('onclick', copy);
    showButton.attachEvent('onclick', toggleShow);
    scanButton.attachEvent('onclick', scan);
}*/

window.onload = function() {

  f_validate('mqip', function(o_event) {
    var a_bytes = this.value.split('.');
    for (var n_byte = 0; n_byte < 4; n_byte++) {
      if (!isNaN(a_bytes[n_byte])) {
        a_bytes[n_byte] = '';
        break;
      }
      else
        a_bytes[n_byte] = Math.abs(parseInt(a_bytes[n_byte]));
    }
    this.value = a_bytes.join('.');
    return true;
  });

  f_getRequest('config', {
    success: function(a_response) {

      f_inputValue('id', a_response.id);
      f_inputValue('ver', a_response.ver);
      f_inputValue('mqip', a_response.mqip);

      f_populateSelect('mqto', ['1 second', 1e3, '2 seconds', 2e3, '3 seconds', 3e3, '5 seconds', 5e3, '10 seconds', 1e4, '30 seconds', 3e4, '1 minute', 6e4, '5 minutes', 3e5], 5e3, a_response.mqto);
      f_populateSelect('rdt', ['twice per second', 5e2, 'every second', 1e3, 'every 2 seconds', 2e3, 'every 3 seconds', 3e3, 'every 5 seconds', 5e3, 'every 10 seconds', 1e4, 'twice per minute', 3e4, 'every minute', 6e4], 1e3, a_response.rdt);
      f_populateSelect('srr', [1, 1, 2, 2, 3, 3, 4, 4, 5, 5], 3, a_response.srr);

      var a_options = ['3%', 3];
      for (n_value = 5; n_value <= 80; n_value += 5) {
        a_options[a_options.length] = n_value + '%';
        a_options[a_options.length] = n_value;
      }
      f_populateSelect('srt', a_options, 15, a_response.srt);

      a_options = [];
      for (n_value = 5; n_value <= 30; n_value++) {
        a_options[a_options.length] = n_value + ' seconds';
        a_options[a_options.length] = n_value * 1000;
      }
      f_populateSelect('mtt', a_options, 1e4, a_response.mtt);

      f_populateSelect('rlt', ['0.2 second', 200, '0.3 second', 300, '0.4 second', 400, '0.5 second', 500, '0.7 second', 700, '1 second', 1e3], a_response.rlt);

      a_options = [];
      for (n_value = 300; n_value <= 1e3; n_value += 100) {
        a_options[a_options.length] = (n_value / 1e3) + ' second';
        a_options[a_options.length] = n_value;
      }
      for (n_value = 2; n_value <= 5; n_value += 1) {
        a_options[a_options.length] = n_value + ' seconds';
        a_options[a_options.length] = n_value * 1000;
      }
      f_populateSelect('rlp', a_options, [], a_response.rlp);
      //console.log(a_response);

      // scan for APs
      window.s_ssid = a_response.ssid;
      f_apScan();
    }
  });


}

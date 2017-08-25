var s_baseUrl = 'http://192.168.0.1/',
  a_networks = [];
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
var o_rsa = new RSAKey(),
  o_val = new c_validator();

function f_boxSwitch(b_display, s_box, s_input) {
  var e_style = f_getById(s_box).style;
  var s_display = b_display ? '' : 'none';
  if (e_style.display == s_display)
    return;
  e_style.display = s_display;
  if (!b_display || !s_input)
    return;
  var e_input = f_getById(s_input);
  e_input.value = '';
  e_input.focus();
}
function f_boxManual(b_display) {
  f_boxSwitch(b_display, 'box-ssid', 'ssid-value');
}
function f_boxPass(b_display) {
  f_boxSwitch(b_display, 'box-pass', 'ap-pass');
}
function f_apChange(n_network) {
  o_val.f_checkValid('pass,ssid');
}
function f_mqttChange() {
  o_val.f_checkValid('mqip,mqpt,name');
}

function f_loadConfig(f_done) {
  var b_success = false;

  f_getRequest('config', {
    success: function(a_response) {

      f_inputValue('id', a_response.id);
      f_inputValue('sys', a_response.sys);
      f_inputValue('ver', a_response.ver);
      f_inputValue('mqtt', a_response.mqtt);
      f_inputValue('mqip', a_response.mqip);

      f_populateSelect('mqto', ['1 second', 1e3, '2 seconds', 2e3, '3 seconds', 3e3, '5 seconds', 5e3, '10 seconds', 1e4, '30 seconds', 3e4, '1 minute', 6e4, '5 minutes', 3e5], 5e3, a_response.mqto);
      f_populateSelect('rdt', ['twice per second', 5e2, 'every second', 1e3, 'every 2 seconds', 2e3, 'every 3 seconds', 3e3, 'every 5 seconds', 5e3, 'every 10 seconds', 1e4, 'twice per minute', 3e4, 'every minute', 6e4], 1e3, a_response.rdt);
      f_populateSelect('srr', [1, 1, 2, 2, 3, 3, 4, 4, 5, 5], 3, a_response.srr);

      var n_value, a_options = [];
      for (n_value in a_securityTypes) {
        a_options[a_options.length] = a_securityTypes[n_value];
        a_options[a_options.length] = n_value;
      }
      f_populateSelect('sec-value', a_options, 4194308);

      a_options = [];
      for (n_value = 1; n_value <= 14; n_value ++) {
        a_options[a_options.length] = n_value;
        a_options[a_options.length] = n_value;
      }
      f_populateSelect('chan-value', a_options, 6);

      a_options = ['3%', 3];
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

      window.s_ssid = a_response.ssid || '';
      b_success = true;
    },
    error: function () {
      f_message('Error Requesting Configuration', true);
    },
    regardless: function() {
      f_done(b_success);
    }
  });
}


function f_apScan() {
  var e_apList = f_getById('aplist');
  e_apList.innerHTML = '<li>Loading...</li>';

  f_boxPass(false);
  f_boxManual(false);

  var s_html = '',
    b_ssidFound = false,
    b_showPassword = true;

  f_getRequest('scan-ap', {
    success: function(a_response) {
      var n_network;
      window.a_networks = a_response.scans;
      for (n_network = 0; n_network < a_networks.length; n_network++)
        a_networks[n_network].value = n_network;
      a_networks.sort(function(a, b) {
        return b.rssi - a.rssi;
      });

      for (n_network = 0; n_network < a_networks.length; n_network++) {
        var a_network = a_networks[n_network];
        if (window.s_ssid == a_network.ssid) {
          b_ssidFound = true;
          if (!a_network.sec)
            b_showPassword = false;
        }
        s_html += '<li><input type="radio" name="ap" value="' + n_network +
          '"' + (window.s_ssid == a_network.ssid ? 'checked' : '') + ' onclick="f_apChange()" /><div>' +
          a_network.ssid + '<span>' + (a_securityTypes[a_network.sec] ?
          a_securityTypes[a_network.sec] + ', ' : '') +
          f_signalStrength(a_network.rssi) + '</span></div></li>';
      }
      if (!n_network)
        s_html += '<li>No networks found</li>';
    },
    error: function () {
      f_message('Error Scanning Networks', true);
    },
    regardless: function() {
      s_html += '<li><input type="radio" name="ap" value="-1"' + (b_ssidFound ? '' : 'checked') +
        ' onclick="f_apChange(this.value)" onclick="f_apChange()" /><div>Enter Manually<span>Use For Hidden or Currently Unavailable Networks</span></div></li>';
      e_apList.innerHTML = s_html;
      o_val.f_checkValid('pass,ssid');
    }
  });
}

function f_loadKey() {
  o_val.f_setValid('pkey', false);
  f_getRequest('public-key', {
    success: function(a_response) {
      var s_publicKey = a_response.b;
      o_rsa.setPublic(s_publicKey.substring(58, 58 + 256), s_publicKey.substring(318, 318 + 6));
      o_val.f_setValid('pkey', true);
    },
    error: function () {
      f_message('Error Requesting Public Key', true);
    }
  });
}

window.onload = function() {

  f_validate('ssid-value', function(o_event) {
    o_val.f_checkValid('ssid');
  });

  f_validate('pass', function(o_event) {
    o_val.f_checkValid('pass');
  });

  f_validate('mqip', function(o_event) {
    var a_bytes = o_event.target.value.replace(/[^\d\.]+/, '').replace('..','.').replace(/^\./, '').split('.');
    for (var n_byte = 0; n_byte < Math.min(4, a_bytes.length); n_byte++) {
      if (a_bytes[n_byte] > 0xFF)
        a_bytes[n_byte] = Math.floor(a_bytes[n_byte] / 10);
    }
    o_event.target.value = a_bytes.join('.');
    o_val.f_checkValid('mqip');
  });

  f_validate('mqpt', function(o_event) {
    var n_value = o_event.target.value;
    n_value = n_value.replace(/\D+/, '');
    if (n_value > 0xFFFF)
      n_value = Math.floor(n_value / 10);
    o_event.target.value = n_value;
    o_val.f_checkValid('mqpt');
  });

  f_validate('name', function(o_event) {
    o_event.target.value = o_event.target.value.replace(/[^\w\d\_]+/, '').substring(0 ,30);
    o_val.f_checkValid('name');
  });

  f_loadConfig(function(b_success) {
    f_mqttChange();
    f_loadKey();
    f_apScan();
  });
};

function f_formSubmit() {
  if (!o_val.f_isValid()) {
    f_message(o_val.f_getErrors().join('</li><li>'), true);
    return false;
  }

  var a_wifi = {
    idx: 0,
    pwd: o_rsa.encrypt(f_inputValue('ap-pass'))
  };

  var n_network = f_inputValue('ap');
  if (n_network >= 0) {
    var a_network = a_networks[n_network];
    a_wifi.ssid = a_network.ssid;
    a_wifi.sec = a_network.sec;
    a_wifi.ch = a_network.ch;
  }
  else {
    a_wifi.ssid = f_inputValue('ssid-value');
    a_wifi.sec = parseInt(f_inputValue('sec-value'));
    a_wifi.ch = parseInt(f_inputValue('chan-value'));
  }
  f_submitValid(false);

  f_message('Saving credentials...');
  f_postRequest('configure-ap', a_wifi, {
    success: function(a_response) {
      console.log(a_response);
      // >> configure garadget
      // >> connect to network?
    },
    error: function(s_status, s_text) {
      console.log(s_status + ': ' + s_text);
      f_message();
      f_message('The configuration command failed<br />Check that you are still well connected to the device\'s WiFi hotspot and retry.', true);
      f_submitValid(true);
    },
    regardless: function() {

    }
  });
  return false;
}

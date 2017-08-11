var s_baseUrl = 'http://192.168.0.1/';
var o_rsa = new RSAKey();

function f_apScan() {
  var s_html = '', e_apList = f_getById('aplist');
  var b_ssidFound = false;
  var b_showPassword = true;
  f_boxPass(false);
  f_boxManual(false);
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
        s_html += '<li><input type="radio" name="ap" value="' + n_network
          + '"' + (window.s_ssid == a_network.ssid ? 'checked' : '') + ' onclick="f_apChange(this.value)" /><div>'
          + a_network.ssid + '<span>' + (a_securityTypes[a_network.sec]
          ? a_securityTypes[a_network.sec] + ', ' : '') + f_signalStrength(a_network.rssi)
          + '</span></div></li>';
      }
      if (!n_network)
        s_html += '<li>No networks found</li>';
    },
    error: function () {
      f_message('Error Scanning Networks', true);
    },
    regardless: function() {
      s_html += '<li><input type="radio" name="ap" value="-1"' + (b_ssidFound ? '' : 'checked')
        + ' onclick="f_apChange(this.value)" /><div>Enter Manually<span>Use For Hidden or Currently Unavailable Networks</span></div></li>';
      e_apList.innerHTML = s_html;
      f_boxPass(b_showPassword);
      f_boxManual(!b_ssidFound);
      f_inputValue('ssid-value', window.s_ssid);
    }
  });
}

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
      f_inputValue('sys', a_response.sys);
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

      // scan for APs
      window.s_ssid = a_response.ssid;
      f_apScan();
    },
    error: function () {
      f_message('Error Requesting Configuration', true);
    },
  });

  window.s_publicKey = '';
  f_getRequest('public-key', {
    success: function(a_response) {
      var s_publicKey = a_response.b;
      o_rsa.setPublic(s_publicKey.substring(58, 58 + 256), s_publicKey.substring(318, 318 + 6));
    },
    error: function () {
      f_message('Error Requesting Public Key', true);
    }
  });

}

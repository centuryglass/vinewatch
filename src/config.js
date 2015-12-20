var basalt_color_val = "";
var debug = true;

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    if(debug)console.log('PebbleKit JS ready!');
    Pebble.sendAppMessage({'request_data':"color"});
    if(debug)console.log('requesting color data');
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    if(debug)console.log('AppMessage received!');
    if('basalt_colors' in e.payload){
      basalt_color_val = JSON.stringify(e.payload.basalt_colors);
      basalt_color_val = basalt_color_val.replace('"','');
      basalt_color_val = basalt_color_val.replace('"','');
      if(debug)console.log('AppMessage ' + basalt_color_val +' received!');
    }
  });
                        
Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  var options = {};
  options.watch_version = 3;
  var url = 'http://centuryglass.github.io/vinewatch/index.html'+
      "?watch_version=3"+
      "&basalt_colors="+basalt_color_val;
  
  if(debug)console.log('opening '+url);
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode and parse config data as JSON
  var config = JSON.parse(decodeURIComponent(e.response));
  var config_out = {};
  if(debug)console.log('Config window returned: '+ JSON.stringify(config.basalt_colors));
  basalt_color_val = JSON.stringify(config.basalt_colors);
  basalt_color_val = basalt_color_val.replace('"','');
  basalt_color_val = basalt_color_val.replace('"','');
  config_out.basalt_colors = config.basalt_colors;
  Pebble.sendAppMessage(config);
  
});
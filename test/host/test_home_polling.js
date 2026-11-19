var fixtureDirectory = arguments[0];
var requests = [], timers = new Map(), now = 0, nextTimer = 0;
var errors = [], alerts = [], mutations = 0, outstanding = 0, maximumOutstanding = 0;
function alert(message) { alerts.push(message); }
var elements = {}, listeners = {};
var console = {log: function(){}, error: function(){errors.push(Array.from(arguments));}};
function assert(condition, message) { if (!condition) throw new Error(message); }
function setTimeout(callback, delay) {
  var id = ++nextTimer;
  timers.set(id, {at: now + delay, callback: callback});
  return id;
}
function clearTimeout(id) { timers.delete(id); }
function advance(milliseconds) {
  var end = now + milliseconds;
  for (;;) {
    var first = null;
    timers.forEach(function(timer, id) {
      if (timer.at <= end && (!first || timer.at < first.timer.at)) first = {id: id, timer: timer};
    });
    if (!first) break;
    now = first.timer.at;
    timers.delete(first.id);
    first.timer.callback();
    drainMicrotasks();
  }
  now = end;
  drainMicrotasks();
}
function AbortController() {
  this.signal = {aborted: false, listeners: [], addEventListener: function(type, callback) {
    assert(type === 'abort', 'unexpected signal event');
    this.listeners.push(callback);
  }};
}
AbortController.prototype.abort = function() {
  this.signal.aborted = true;
  this.signal.listeners.forEach(function(callback){callback();});
};
function fetch(url, options) {
  options = options || {};
  return new Promise(function(resolve, reject) {
    var request = {url: url, options: options, active: true};
    requests.push(request);
    if (url === '/home_state') {
      outstanding++;
      maximumOutstanding = Math.max(maximumOutstanding, outstanding);
    }
    request.finish = function() {
      assert(request.active, 'request completed twice');
      request.active = false;
      if (url === '/home_state') outstanding--;
    };
    request.respond = function(status, data, jsonError) {
      request.finish();
      resolve({ok: status >= 200 && status < 300, status: status, json: function() {
        return jsonError ? Promise.reject(new Error('Invalid JSON')) : Promise.resolve(data);
      }});
      drainMicrotasks();
    };
    request.fail = function(message) {
      request.finish();
      reject(new Error(message));
      drainMicrotasks();
    };
    if (options.signal) options.signal.addEventListener('abort', function() {
      if (request.active) request.fail('AbortError');
    });
  });
}
var document = {
  hidden: false,
  readyState: 'loading',
  getElementById: function(id) { return elements[id] || null; },
  addEventListener: function(type, callback) {
    (listeners[type] || (listeners[type] = [])).push(callback);
  }
};
function emit(type) { (listeners[type] || []).forEach(function(callback){callback();}); drainMicrotasks(); }
function element(text, classes, color) {
  var result = {visible: true, getClientRects: function(){return this.visible ? [{}] : [];}};
  var storedText = text, storedColor = color || '', storedClasses = new Set(classes || []);
  Object.defineProperty(result, 'textContent', {
    get: function(){return storedText;},
    set: function(value){mutations++; storedText = value;}
  });
  result.style = {};
  Object.defineProperty(result.style, 'backgroundColor', {
    get: function(){return storedColor;},
    set: function(value){mutations++; storedColor = value;}
  });
  result.classList = {
    contains: function(name){return storedClasses.has(name);},
    add: function(name){mutations++; storedClasses.add(name);},
    remove: function(name){mutations++; storedClasses.delete(name);}
  };
  return result;
}
function populateHome() {
  for (var i = 0; i < 2; i++) {
    elements['cover'+i+'-position'] = element('0%');
    elements['cover'+i+'-status'] = element('Status: Gestoppt');
    elements['cover'+i+'-open'] = element('▲ Öffnen');
    elements['cover'+i+'-close'] = element('▼ Schließen');
  }
  for (var r = 4; r < 24; r++) elements['relay-'+r] = element('Lampe: Name: AUS', ['btn', 'btn-off']);
  for (var n = 0; n < 16; n++) elements['input-'+n+'-status'] = element('CLOSED', [], 'lightgreen');
}
function fresh(home, hidden) {
  requests.filter(function(request){return request.active;}).forEach(function(request){request.fail('test cleanup');});
  timers.clear();
  now = 0; elements = {}; requests = []; errors = []; alerts = []; mutations = 0;
  outstanding = 0; maximumOutstanding = 0;
  homePollTimer = null; homePollInFlight = false; homePollRefreshPending = false; homePollDelay = 1000;
  document.hidden = !!hidden;
  if (home) populateHome();
}

load(fixtureDirectory + '/home.js');
assert(requests.length === 0 && timers.size === 0, 'must wait for DOMContentLoaded');
var state = JSON.parse(readFile(fixtureDirectory + '/home-state.json'));
var inputs = JSON.parse(readFile(fixtureDirectory + '/inputs.json'));
assert(validHomeState(state), 'actual bundled API response rejected');
assert(state.relays.length === 24 && state.covers.length === 2 && state.inputs.length === 16, 'API shape');
assert(JSON.stringify(state.inputs) === JSON.stringify(inputs.inputs), '/inputs compatibility');
for (var index = 0; index < 24; index++) assert(state.relays[index] === ((35 >> (index % 5)) & 1), 'relay value');
for (var index = 0; index < 16; index++) assert(state.inputs[index] === ((35 >> (index % 5)) & 1), 'input value');
assert(state.covers[0].position === 100 && state.covers[1].position === 0, 'cover positions');
assert(state.covers[0].resetPending === true && state.covers[1].resetPending === false, 'reset flags');
assert(state.covers[0].openRelay === state.relays[0] && state.covers[0].closeRelay === state.relays[1], 'cover relay states');

populateHome();
document.readyState = 'complete';
emit('DOMContentLoaded');
assert(requests.length === 1 && requests[0].url === '/home_state', 'one bundled initial request');
assert(requests[0].options.cache === 'no-store', 'status must not be cached');
for (var i = 0; i < 10; i++) updateHomeState();
assert(requests.length === 1, 'overlapping refreshes must coalesce');
requests[0].respond(200, state);
advance(0);
assert(requests.length === 2, 'one queued refresh after in-flight response');
requests[1].respond(200, state);
advance(999);
assert(requests.length === 2, 'poll must wait 1000 ms after completion');
advance(1);
assert(requests.length === 3 && maximumOutstanding === 1, 'one periodic request, no overlap');
requests[2].respond(200, state);
assert(requests.every(function(request){return request.url === '/home_state';}), 'no separate /inputs polling');

fresh(true, false);
applyHomeState(state);
assert(mutations > 0, 'changed values must render');
var afterFirst = mutations;
applyHomeState(state);
assert(mutations === afterFirst, 'unchanged data must not mutate DOM');
elements['relay-4'].textContent = 'Lampe: Name: FALSCH';
applyHomeState(state);
assert(elements['relay-4'].textContent === 'Lampe: Name: AUS', 'must correct optimistic/stale DOM without a cached-state shortcut');
elements['input-0-status'].visible = false;
elements['input-0-status'].textContent = 'HIDDEN';
applyHomeState(state);
assert(elements['input-0-status'].textContent === 'HIDDEN', 'hidden element must not update');
elements['input-0-status'].visible = true;
applyHomeState(state);
assert(elements['input-0-status'].textContent === 'OPEN', 'newly visible element must catch up');
assert(elements['cover0-status'].textContent === 'Status: Reset fährt auf 100%', 'reset status preserved');

fresh(false, false);
updateHomeState(); emit('visibilitychange');
assert(requests.length === 0 && timers.size === 0, 'other pages must not poll');
fresh(true, true);
updateHomeState();
assert(requests.length === 0 && timers.size === 0, 'hidden tab must not poll');
document.hidden = false; emit('visibilitychange');
assert(requests.length === 1, 'visible tab refreshes immediately');
document.hidden = true; emit('visibilitychange');
var beforeHiddenResponse = mutations;
requests[0].respond(200, state);
assert(mutations === beforeHiddenResponse && timers.size === 0, 'in-flight hidden response must not render or reschedule');
document.hidden = false; emit('visibilitychange');
requests[1].respond(200, state);
document.hidden = true; emit('visibilitychange');
advance(10000);
assert(requests.length === 2, 'hiding cancels pending timer');

fresh(true, false);
updateHomeState();
requests[0].respond(503, state);
assert(errors.length === 1 && homePollDelay === 2000, 'HTTP errors logged and backed off');
advance(1999); assert(requests.length === 1, 'no early HTTP retry');
advance(1); requests[1].respond(200, null, true);
assert(errors.length === 2 && homePollDelay === 4000, 'JSON errors backed off');
advance(4000); requests[2].respond(200, {relays: [], covers: [], inputs: []});
assert(errors.length === 3 && homePollDelay === 8000, 'invalid shape backed off');
advance(8000);
updateHomeState();
requests[3].fail('Network failure');
assert(homePollDelay === 8000, 'backoff capped at 8000 ms');
advance(0); assert(requests.length === 4, 'queued refresh must not bypass failure backoff');
advance(8000); requests[4].respond(200, state);
assert(homePollDelay === 1000, 'success resets backoff');
advance(1000);
assert(requests.length === 6 && maximumOutstanding === 1, 'polling recovers without overlap');

fresh(true, false);
updateHomeState();
advance(4999);
assert(requests[0].active && requests.length === 1, 'request timeout must be 5000 ms');
advance(1);
assert(!requests[0].active && errors.length === 1 && !homePollInFlight, 'timeout aborts and releases request');
advance(1999); assert(requests.length === 1, 'timeout retry respects backoff');
advance(1); requests[1].respond(200, state);
assert(maximumOutstanding === 1, 'timeout retry does not overlap');

fresh(true, false);
updateHomeState();
coverAction(0, 'STOP', null);
assert(requests[1].url === '/cover?idx=0&action=STOP', 'cover action route preserved');
requests[1].respond(200, {success: true});
assert(requests.length === 2, 'cover refresh coalesces with in-flight status');
requests[0].respond(200, state);
advance(0);
assert(requests.length === 3 && requests[2].url === '/home_state', 'cover action gets fresh state after in-flight poll');
requests[2].respond(200, state);
assert(maximumOutstanding === 1, 'cover action must not create parallel status requests');

print('PASS: bundled API values, DOM readiness, single-flight/coalescing, 1000 ms polling, changed/visible DOM only, tab visibility, HTTP/JSON/schema/network backoff (2000/4000/8000 ms), 5000 ms timeout and cover refresh');

fresh(false, false);
elements['led-brightness'] = element('0%');
elements['ac-brightness'] = element('0%');
var led = createDimmerSender('/led', 'led-brightness');
var ac = createDimmerSender('/kronleuchter', 'ac-brightness');
for (var value = 0; value <= 100; value++) led(String(value), false);
assert(elements['led-brightness'].textContent === '100%', 'slider feedback must be immediate');
advance(249); assert(requests.length === 0, 'debounce must wait 250 ms');
advance(1); assert(requests.length === 1 && requests[0].url === '/led?brightness=100&ajax=1', 'only latest slider value sent');
led('25', true);
assert(requests.length === 1 && elements['led-brightness'].textContent === '25%', 'final value waits for in-flight request');
ac('33', true);
assert(requests.length === 2 && requests[1].url === '/kronleuchter?brightness=33&ajax=1', 'dimmers operate independently');
requests[0].respond(200, {success: true});
assert(requests.length === 3 && requests[2].url === '/led?brightness=25&ajax=1', 'final value sent after old value completes');
requests[1].respond(200, {success: true});
requests[2].respond(200, {success: true});
led('25', true);
assert(requests.length === 3, 'acknowledged final value not duplicated');
led('0', true);
assert(requests[3].url === '/led?brightness=0&ajax=1', 'final zero reliably sent');
requests[3].respond(200, {success: true});
led('10', true);
requests[4].fail('Network failure');
advance(999); assert(requests.length === 5, 'retry delayed by 1000 ms');
advance(1); assert(requests.length === 6 && requests[5].url === '/led?brightness=10&ajax=1', 'one bounded final-value retry');
requests[5].fail('Network failure');
advance(10000);
assert(requests.length === 6 && alerts.length === 1, 'persistent failure surfaced without infinite retries');
led('10', true);
assert(requests.length === 7, 'user can retry same value after failure');
advance(5000);
assert(!requests[6].active, 'dimmer request times out');
advance(1000);
requests[7].respond(200, {success: true});
print('PASS: 250 ms dimmer debounce, immediate labels, independent serialized requests, reliable final/zero values, JSON AJAX, bounded retry and timeout');

var web = JSON.parse(readFile(fixtureDirectory + '/web-escaping.json'));
assert((web.edit.match(/maxlength='30'/g) || []).length === 41, 'all existing name input limits preserved');
function attributeValue(html, id) {
  var match = html.match(new RegExp("id='" + id + "' value='([^']*)'"));
  assert(match, 'missing value attribute: ' + id);
  var entities = {amp: '&', lt: '<', gt: '>', quot: '"', '#39': "'"};
  return match[1].replace(/&(amp|lt|gt|quot|#39);/g, function(all, entity){return entities[entity];});
}
for (var index = 0; index < 3; index++) assert(attributeValue(web.edit, 'specialname'+index) === web.name, 'special name round-trip');
for (var index = 0; index < 2; index++) assert(attributeValue(web.edit, 'covername'+index) === web.name, 'cover name round-trip');
for (var index = 4; index < 24; index++) assert(attributeValue(web.edit, 'name'+index) === web.name, 'relay name round-trip');
for (var index = 0; index < 16; index++) assert(attributeValue(web.edit, 'inputname'+index) === web.name, 'input name round-trip');
['mqtt_broker', 'mqtt_user', 'mqtt_pass'].forEach(function(id) {
  assert(attributeValue(web.mqtt, id) === web.name, 'MQTT attribute round-trip: ' + id);
});
assert(web.mqtt.indexOf('topic&amp;base') !== -1, 'MQTT topic text escaped');
['espnow', 'clientList', 'clientDetail'].forEach(function(page) {
  assert(web[page].indexOf(web.name) === -1 && web[page].indexOf('&lt;tag&gt;&quot;&#39;&amp;\\ Tür') !== -1,
         'ESP-NOW name escaped on ' + page);
});
web.saves.forEach(function(response) {
  assert(response.success && response.name === web.name && response.enabled === true, 'save JSON must preserve raw name and HA flag');
});
assert(web.controls.length === 32, 'JSON must round-trip all control bytes');
for (var index = 0; index < 32; index++) assert(web.controls.charCodeAt(index) === index, 'JSON control byte '+index);

fresh(false, false);
load(fixtureDirectory + '/edit.js');
['name4', 'covername0', 'inputname0', 'specialname0'].forEach(function(id) {
  elements[id] = {value: attributeValue(web.edit, id)};
});
['enabled4', 'coverenabled0', 'inputenabled0', 'specialenabled0'].forEach(function(id) {
  elements[id] = {checked: true};
});
elements.type4 = elements.inputtype0 = {value: 'switch'};
saveConfig(4); saveCoverConfig(0); saveInputConfig(0); saveSpecialConfig(0);
assert(requests.length === 4, 'all four Edit save buttons send requests');
requests.forEach(function(request, index) {
  var name = request.url.match(/[?&]name=([^&]*)/)[1];
  assert(decodeURIComponent(name) === web.name, 'save must send raw name, not HTML entities');
  request.respond(200, web.saves[index]);
});
assert(alerts.length === 4 && alerts.every(function(message){return message.indexOf('gespeichert') !== -1;}), 'valid JSON names must show save success');
['name4', 'covername0', 'inputname0', 'specialname0'].forEach(function(id){elements[id].value = '   ';});
saveConfig(4); saveCoverConfig(0); saveInputConfig(0); saveSpecialConfig(0);
assert(requests.length === 4 && alerts.length === 8, 'empty-name validation preserved for every Save button');
print('PASS: all 41 Edit name fields, MQTT attributes, three ESP-NOW views, UTF-8/HTML/JSON round-trips, control bytes and all four real Edit save scripts');

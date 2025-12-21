export async function listPorts() {
  return await window.api.serial.list();
}

export async function connectPort(path) {
  return await window.api.serial.connect(path);
}

export async function disconnectPort() {
  return await window.api.serial.disconnect();
}

export function onSerialLog(cb) {
  window.api.serial.onLog(cb);
}

const { contextBridge, ipcRenderer } = require("electron");

contextBridge.exposeInMainWorld("api", {
  serial: {
    list: () => ipcRenderer.invoke("serial:list"),
    connect: path => ipcRenderer.invoke("serial:connect", path),
    disconnect: () => ipcRenderer.invoke("serial:disconnect"),
    onLog: cb => ipcRenderer.on("serial:log", (_, data) => cb(data))
  }
});

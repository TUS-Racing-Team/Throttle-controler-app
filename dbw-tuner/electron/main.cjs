const { app, BrowserWindow, Menu, ipcMain } = require("electron");
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");

let win;
let port;
let parser;

function createWindow() {
  Menu.setApplicationMenu(null);

  win = new BrowserWindow({
    width: 1200,
    height: 850,
    backgroundColor: "#0b0f14",
    webPreferences: {
      preload: __dirname + "/preload.cjs",
      contextIsolation: true
    }
  });

  win.loadURL("http://localhost:5173");
  win.webContents.on("context-menu", e => e.preventDefault());
}

app.whenReady().then(createWindow);

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") app.quit();
});

/* ---------- SERIAL ---------- */

ipcMain.handle("serial:list", async () => {
  const ports = await SerialPort.list();
  return ports.map(p => ({
    path: p.path,
    manufacturer: p.manufacturer || ""
  }));
});

ipcMain.handle("serial:connect", async (_, path) => {
  if (port) {
    port.close();
    port = null;
  }

  return new Promise((resolve, reject) => {
    port = new SerialPort({
      path,
      baudRate: 115200,
      autoOpen: false
    });

    port.open(err => {
      if (err) return reject(err.message);

      parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

      parser.on("data", line => {
        const msg = line.trim();

        let level = "info";
        if (msg.startsWith("WARN:")) level = "warn";
        if (msg.startsWith("ERR:")) level = "error";

        win.webContents.send("serial:log", {
          level,
          message: msg.replace(/^(LOG:|WARN:|ERR:)\s*/, "")
        });
      });

      // handshake
      port.write("HELLO\n");

      resolve(true);
    });
  });
});

ipcMain.handle("serial:disconnect", () => {
  if (port) port.close();
  port = null;
  return true;
});

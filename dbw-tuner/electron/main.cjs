const { app, BrowserWindow, Menu } = require("electron");

function createWindow() {
  // махаме File / Edit / View менюто
  Menu.setApplicationMenu(null);

  const win = new BrowserWindow({
    width: 1200,
    height: 850,
    backgroundColor: "#0b0f14",
    webPreferences: {
      contextIsolation: true
    }
  });

  win.loadURL("http://localhost:5173");

  // махаме десния бутон (context menu)
  win.webContents.on("context-menu", e => {
    e.preventDefault();
  });

  // ⛔ НЯМА openDevTools()
}

app.whenReady().then(createWindow);

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") app.quit();
});

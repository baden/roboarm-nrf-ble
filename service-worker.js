self.addEventListener('install', (event) => {
  console.log('[Service Worker] Installed');
  self.skipWaiting();
});

self.addEventListener('activate', (event) => {
  console.log('[Service Worker] Activated');
});

self.addEventListener('fetch', (event) => {
  // Проста обробка запитів (тільки кешування за замовчуванням)
});

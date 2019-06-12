# http-server-display-info-from-arduino

## Принцип работы

При детектировании нажатия или вибрации *arduino* отправляет **HTTP POST** запрос веб-приложению для обновления данных, а также издаёт звуковой сигнал, включает соответствующий светодиод и отобржает данные о количестве детектированных событий на дисплей.

На главной странице сайта переодически происходит обновление данных с помощью технологии *WEB Sockets*.

Проект доступен на [*Heroku*](https://afternoon-ravine-88100.herokuapp.com/)

## Используемые датчики и модули

- [датчик вибрации](https://roboshop.spb.ru/SW-420-module)

- [датчик касания](https://roboshop.spb.ru/touch-sensor-1)

- [RGB светодиод](https://roboshop.spb.ru/RGB-led-module). Мигает при детектировании нажатия(*синим*) или вибрации(*зелёным*). Во время работы горит *красным* цветом.

- [Динамик](https://amperkot.ru/spb/catalog/pezodinamik_buzzer_passivnyiy__1_sht-24363255.html). Пищит при детектировании нажатия или вибрации и при включении.

- [7-ми сегментный дисплей на 4 цифры](https://ru.aliexpress.com/item/32401419391.html?cv=47843&ws_ab_test=searchweb201556_6_79_78_77_80%25252Csearchweb201644_5%25252Csearchweb201560_7&af=44981&mall_affr=pr3&cn=43pnecycezkvh4vbgzaow3b494ny3if0&dp=036435a786997d07caa63d7d19c5195c&tmLog=onelink_blank&scm=1007.22893.125764.0&pvid=a0dcc2aa-d9a8-44a7-bba2-c09ab5d0319e&onelink_thrd=0.0&onelink_page_from=ITEM_DETAIL&onelink_item_to=32401419391&onelink_duration=0.897048&onelink_status=noneresult&onelink_item_from=32401419391&onelink_page_to=ITEM_DETAIL&afref=&aff_platform=aaf&cpt=1560376203897&sk=VnYZvQVf&aff_trace_key=dba8bcda7a214eb6a04cb5f04a3d528b-1560376203897-07114-VnYZvQVf&terminal_id=bc1802624155481cbdbc6e33efa4744d). Служит для отображения количества детектированных событий.

- Модуль [Ethernet W5500 SPI](https://amperkot.ru/products/plata_setevaya_ethernet_w5500_spi/24346551.html) для работы с сетью.

## Используемые библиотеки

- Для отображения информации на дисплее используется [библиотека](https://alexgyver.ru/tm74hc595_display/) автора [AlexGyver](https://github.com/AlexGyver).

- Для демонстрации уведомлений на сайте исползуется библиотека [Notify.js](https://notifyjs.jpillora.com/)

- Для работы с *WEB Sockets* библиотека [Socket.io](https://socket.io/)

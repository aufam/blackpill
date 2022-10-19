#include "project.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::OS;
using namespace Project::DSP;
extern char blink_symbol[16];

void mainThread(void *arg);
void project_init() {
    static ThreadStatic<4096> thread;
    thread.init("Main Thread", osPriorityAboveNormal, mainThread);
}

enum { EVENT_CLEAR, EVENT_BT_UP, EVENT_BT_DOWN, EVENT_BT_ROT, EVENT_SCROLL_UP, EVENT_SCROLL_DOWN };
QueueStatic<int, 1> event;
Oled oled{ i2c1 };
Audio audio{ i2s2, AUDIO_EN_GPIO_Port, AUDIO_EN_Pin };
Buzzer buzzer{ pwm2, TIM_CHANNEL_1, 100 };
auto &encoder = encoder4;
auto &uart = uart2;
String<32> f;

struct Option {
    uint8_t col = 0, row = 0;
    char *(*text)() = nullptr;
    void (*pressed)() = nullptr;
    Oled::Font font = Adafruit5x7;
    mutable bool isSelected = false;
    void print() const {
        oled.setFont(font);
        oled.print(text(), isSelected, col, row);
    }
};

void setClock();
void setDate();
void menu();
    void setBlink();
    void encode();
    void encodeTest();
    void record();
    void playFromPC();
    void playSineWave();
    void testLinkedList();

void mainThread(void *arg) {
    (void) arg;

    exti.setCallback(SW_A_Pin, [](void *) { event << EVENT_BT_UP; });
    exti.setCallback(SW_B_Pin, [](void *) { event << EVENT_BT_DOWN; });
    exti.setCallback(SW_ROT_Pin, [](void *) { event << EVENT_BT_ROT; });
    encoder.init(
            [](void *){ event << EVENT_SCROLL_UP; }, nullptr,
            [](void *){ event << EVENT_SCROLL_DOWN; }, nullptr);

    rtc.init();
    event.init();
    oled.init();
    audio.init();
    uart.init();
    buzzer.init(SystemCoreClock / 1_M - 1, 1_k - 1, 500);

    const Option options[] = {
            /* clock */ {
                35, 1,
                [](){ return f(" %02d:%02d:%02d", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()); },
                setClock
            },
            /* date */ {
                25, 2,
                [](){ return f(" %s %02d/%02d/%02d", rtc.getDay(), rtc.getDate(), rtc.getMonth(), rtc.getYear()); },
                setDate
            },
            /* menu */ {
                92, 7,
                [](){ return f(" Menu"); },
                menu
            }
    };
    int16_t optionIndex = 1;
    const int optionIndexMax = sizeof(options) / sizeof(Option) - 1;

    for(;;) {
        for (int i = 0; i <= optionIndexMax; i++) {
            options[i].isSelected = i == optionIndex;
            options[i].print();
        }

        int evt = EVENT_CLEAR;
        event.pop(evt, 500);
        switch (evt) {
            default:
            case EVENT_CLEAR: break;
            case EVENT_SCROLL_DOWN:
                if (optionIndex > 0) optionIndex--;
                break;
            case EVENT_SCROLL_UP:
                if (optionIndex < optionIndexMax) optionIndex++;
                break;
            case EVENT_BT_ROT:
                buzzer.start();
                oled.clear();
                options[optionIndex].pressed();
                oled.clear();
                break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                break;
        }
    }
}

void menu() {
    const Option options[] = {
            { 0, 0, [](){ return f(" Record");},          record },
            { 0, 1, [](){ return f(" Encode");},          encode },
            { 0, 2, [](){ return f(" Test MBE Encode");}, encodeTest },
            { 0, 3, [](){ return f(" Play From PC");},    playFromPC },
            { 0, 4, [](){ return f(" Play Sine Wave"); }, playSineWave },
            { 0, 5, [](){ return f(" Set Blink"); },      setBlink},
            { 0, 6, [](){ return f(" Linked List"); },    testLinkedList},
    };
    int optionIndex = 0;
    const int optionIndexMax = sizeof(options) / sizeof(Option) - 1;

    for(;;) {
        for (int i = 0; i <= optionIndexMax; i++) {
            options[i].isSelected = i == optionIndex;
            options[i].print();
        }

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        switch (evt) {
            default:
            case EVENT_CLEAR: break;
            case EVENT_SCROLL_DOWN: if (optionIndex > 0) optionIndex--; break;
            case EVENT_SCROLL_UP: if (optionIndex < optionIndexMax) optionIndex++; break;
            case EVENT_BT_ROT:
                buzzer.start();
                oled.clear();
                options[optionIndex].pressed();
                oled.clear();
                break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                return;
        }
    }
}

void record() {
    oled << " Record and send to PC ...\n";
    auto &audioBuffer = *(Audio::BufferMono *) &usb.rxBuffer;
    for (;;) {
        audio.read(audioBuffer);
        usb.transmit(audioBuffer.data(), Audio::BufferMono::len() * sizeof (Audio::Mono));

        int evt = EVENT_CLEAR;
        event >> evt;
        switch (evt) {
            default: break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                return;
        }
    }
}

void encode() {
    oled << " Encode incoming audio data from PC and send to PC\n";

    using AudioEvent = QueueStatic<Audio::Mono *, 1>;
    AudioEvent audioEvent;
    audioEvent.init();
    Codec codec;
    uint8_t bytes[9];

    usb.setRxCallback(
            [](void *arg, size_t) {
                if (arg == nullptr) return;
                auto &audioEvent = *(AudioEvent *) arg;
                audioEvent << (Audio::Mono *) usb.rxBuffer.begin();
            }, &audioEvent);

    const char *errorCodes[] = {
            "No error",
            "Run time error",
            "Timeout error",
            "Resource not available",
            "Parameter Error",
            "System is out of memory",
            "Not allowed in ISR context",
    };
    for (;;) {
        Audio::Mono *audioBuffer = nullptr;
        int res = audioEvent.pop(audioBuffer, 1000);
        if (res < osOK && res >= osErrorISR) oled << f("%s\r", errorCodes[-res]);
        else {
            auto start = osKernelGetTickCount();
            codec.encode(audioBuffer, bytes);
            auto end = osKernelGetTickCount();
            oled << f("Time elapsed: %ld ms\r", end - start);
            usb.transmit(bytes, sizeof (bytes));
        }

        int evt = EVENT_CLEAR;
        event >> evt;
        switch (evt) {
            default: break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                usb.setRxCallback(nullptr); // reset rx callback
                audioEvent.deinit();
                return;
        }
    }
}

void encodeTest() {
    oled << " Test MBE Encode...\n";

    Codec codec;
    uint8_t bytes[9];
    auto &audioBuffer = *(Audio::BufferMono *) &usb.rxBuffer;

    for (;;) {
        audio.read(audioBuffer);

        auto start = osKernelGetTickCount();
        codec.encode(audioBuffer.data(), bytes);
        auto end = osKernelGetTickCount();
        oled << f("Time elapsed: %ld ms\r", end - start);

        int evt = EVENT_CLEAR;
        event >> evt;
        switch (evt) {
            default: break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                return;
        }
    }
}

void playFromPC() {
    oled << "Playing from PC ...\n";
    const char *request = "request\n\r";
    const char *stop = "stop   \n\r";
    const size_t len = 10;

    auto &audioBuffer = *(Audio::BufferMono *) &usb.rxBuffer;
    audio.speakerEnable();
    for (;;) {
        usb.transmit(request, len);
        audio.write(audioBuffer);

        int evt = EVENT_CLEAR;
        event >> evt;
        switch (evt) {
            default: break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                usb.transmit(stop, len);
                buzzer.start();
                audio.speakerDisable();
                return;
        }
    }
}

void playSineWave() {
    oled << " Playing sine wave...\n";

    int32_t toneFreq = 1000;
    struct VCOAndVolume{
        VCO vco{1000, audio.i2s.sampRate};
        int volume = 1;
        bool stop = false;
    } vcoAndVolume;
    auto &vco = vcoAndVolume.vco;
    auto &volume = vcoAndVolume.volume;

    ThreadStatic<128> audioThread;
    auto audioFn = [](void *arg) {
        auto vcoAndVolume = (VCOAndVolume *) arg;
        auto &vco = vcoAndVolume->vco;
        auto &volume = vcoAndVolume->volume;
        auto &audioBuffer = *(Audio::BufferMono *) &usb.rxBuffer;
        for (; !vcoAndVolume->stop;) {
            for (size_t i = 0; i < Audio::BufferMono::len(); ++vco, i++)
                audioBuffer[i] = (int16_t) (vco.sin().i * 0x100 * volume / 100);
            audio.write(audioBuffer);
        }
    };
    audioThread.init("Audio Thread", osPriorityAboveNormal, audioFn, &vcoAndVolume);

    bool editMode = false;
    int index = 0;
    audio.speakerEnable();
    for (;;) {
        oled.setCursor(0, 2);
        oled.print(f(" Tone: %4d Hz\n", toneFreq), index % 2 == 0);
        oled.print(f(" volume: %d %%\n", volume), index % 2 != 0);

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        switch (evt) {
            default: break;
            case EVENT_BT_ROT:
                buzzer.start();
                editMode = !editMode;
                break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                audio.speakerDisable();
                vcoAndVolume.stop = true;
                audioThread.deinit();
                return;
            case EVENT_SCROLL_UP:
                if (!editMode) index++;
                else if (index % 2 == 0) {
                    if (toneFreq < 4000) toneFreq += 50;
                    vco.setDeviation(toneFreq);
                }
                else {
                    if (volume < 100) volume++;
                }
                break;
            case EVENT_SCROLL_DOWN:
                if (!editMode) index--;
                else if (index % 2 == 0) {
                    if (toneFreq >= 500) toneFreq -= 50;
                    vco.setDeviation(toneFreq);
                }
                else {
                    if (volume > 0) volume--;
                }
                break;
        }
    }
}

void setBlink() {
    const int sizeofBlinkSymbols = sizeof(blink_symbol);
    char blinkSymbols[sizeofBlinkSymbols];
    strcpy(blinkSymbols, blink_symbol);
    const char optionSymbols[] = {'\0', '0', '1'};
    const int optionSymbolsIndexMax = sizeof(optionSymbols) - 1;
    int index = 0;
    bool editMode = false;
    for (;;) {
        oled.setCursor(10, 2);
        int indexMax = (int) strlen(blinkSymbols);
        for (int i = 0; i < sizeofBlinkSymbols; i++) {
            char ch = i < indexMax ? blinkSymbols[i] : i == indexMax ? '_' : ' ';
            oled.print(ch, i == index);
        }

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        switch (evt) {
            default:
            case EVENT_CLEAR: break;
            case EVENT_SCROLL_DOWN:
                if (editMode) {
                    int optionSymbolsIndex = blinkSymbols[index] == '0' ? 1 : blinkSymbols[index] == '1' ? 2 : 0;
                    if (optionSymbolsIndex > 0) blinkSymbols[index] = optionSymbols[optionSymbolsIndex - 1];
                }
                else if (index > 0) index--;
                break;
            case EVENT_SCROLL_UP:
                if (editMode) {
                    int optionSymbolsIndex = blinkSymbols[index] == '0' ? 1 : blinkSymbols[index] == '1' ? 2 : 0;
                    if (optionSymbolsIndex < optionSymbolsIndexMax) blinkSymbols[index] = optionSymbols[optionSymbolsIndex + 1];
                }
                else if (index < indexMax) index++;
                break;
            case EVENT_BT_ROT:
                buzzer.start();
                editMode = !editMode;
                break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                strcpy(blink_symbol, blinkSymbols);
                return;
        }
    }
}

void setClock() {
    uint8_t vals[] = {rtc.getHours(), rtc.getMinutes(), rtc.getSeconds() };
    size_t index = 0;
    int scrollNumber = 0;
    for (;;) {
        oled.setCursor(35, 1);
        oled.print(f(" %02d:", vals[0]), index == 0);
        oled.print(f("%02d:", vals[1]), index == 1);
        oled.print(f("%02d", vals[2]), index == 2);

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        switch (evt) {
            default:
            case EVENT_CLEAR: break;
            case EVENT_SCROLL_DOWN:
                if (scrollNumber) {
                    if (vals[index] > 0) vals[index]--;
                }
                else if (index > 0) index--;
                break;
            case EVENT_SCROLL_UP:
                if (scrollNumber) {
                    int maxIndex = index == 0 ? 23 : 59;
                    if (vals[index] < maxIndex) vals[index]++;
                }
                else if (index < sizeof(vals) - 1) index++;
                break;
            case EVENT_BT_ROT:
                buzzer.start();
                scrollNumber = !scrollNumber;
                break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                rtc.setTime(vals[2], vals[1], vals[0]);
                return;
        }
    }
}

void setDate() {
    uint8_t vals[] = {rtc.getWeekDay(), rtc.getDate(), rtc.getMonth(), rtc.getYear() };
    size_t index = 0;
    bool editMode = false;
    for (;;) {
        oled.setCursor(25, 2);
        oled.print(f(" %s ", RealTimeClock::days[vals[0]]), index == 0);
        oled.print(f("%02d/", vals[1]), index == 1);
        oled.print(f("%02d/", vals[2]), index == 2);
        oled.print(f("%02d", vals[3]), index == 3);

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        switch (evt) {
            default:
            case EVENT_CLEAR: break;
            case EVENT_SCROLL_DOWN:
                if (editMode) {
                    uint8_t minIndices[] = { 0, 1, 1, 0 };
                    uint8_t minIndex = minIndices[index];
                    if (vals[index] > minIndex) vals[index]--;
                }
                else if (index > 0) index--;
                break;
            case EVENT_SCROLL_UP:
                if (editMode) {
                    uint8_t maxIndices[] = { 6, 31, 12, 99 };
                    uint8_t maxIndex = maxIndices[index];
                    if (vals[index] < maxIndex) vals[index]++;
                }
                else if (index < sizeof(vals) - 1) index++;
                break;
            case EVENT_BT_ROT:
                buzzer.start();
                editMode = !editMode;
                break;
            case EVENT_BT_UP:
            case EVENT_BT_DOWN:
                buzzer.start();
                rtc.setDate(vals[0], vals[1], vals[2], vals[3]);
                return;
        }
    }
}

void testLinkedList() {
    oled << "Test linked list\n";
    LinkedList<int> list;
    int dummy;
    for(;;) {
        oled.setCursor(0, 1);
        oled << f("cnt = %d\n", list.len());

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        switch (evt) {
            default:
            case EVENT_CLEAR: break;
            case EVENT_BT_ROT:
            case EVENT_SCROLL_DOWN:
            case EVENT_SCROLL_UP:
                buzzer.start();
                list.push(evt);
                break;
            case EVENT_BT_UP:
                buzzer.start();
                list.pop(dummy);
                break;
            case EVENT_BT_DOWN:
                buzzer.start();
                list.clear();
                return;
        }

        for (auto node = list.head; node != nullptr; node = node->next) oled << f("%d ", node->item);
        oled << '\r';
    }

}
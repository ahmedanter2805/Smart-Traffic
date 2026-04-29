# 📘 دليل دراسة مشروع Smart-Traffic (جامعة - OOP)
> **إعداد: Antigravity AI**

مرحباً يا شباب! هذا الدليل مقسم لـ 4 أجزاء بحيث يتخصص كل فرد منكم في جزء معين ليكون "خبيراً" فيه أمام الدكتور، مع فهم عام لباقي المشروع.

---

## 📑 الفهرس (اضغط على اسمك للانتقال للجزء الخاص بك)

1. [👤 أحمد - مهندس النظام (Strategy & Control)](#ahmed-section)
2. [👤 كريم - خبير الكائنات (Vehicle Hierarchy)](#kareem-section)
3. [👤 مازن - مسؤول البنية التحتية (Infrastructure & Stats)](#mazen-section)
4. [👤 مينا - مطور الواجهة والمحاكاة (UI & Logic Flow)](#mina-section)
5. [📥 طريقة تحميل المشروع من GitHub](#download-section)

---

<a name="ahmed-section"></a>
## 👤 1. أحمد: مهندس النظام (System Architect)
**تركيزك الأساسي:** الـ Brain بتاع المشروع والـ Strategy Pattern.

### 🛠️ الملفات المسؤولة عنها:
- `TrafficStrategy.h / .cpp`
- `TrafficController.h / .cpp`

### 💡 الأسئلة التي يجب أن تجيب عليها:
*   **ما هو الـ Strategy Pattern؟** هو تصميم يسمح لنا بتغيير "طريقة تفكير" الإشارة في وقت التشغيل (Runtime). عندنا `FixedTimeStrategy` (تقليدي) و `SmartAdaptiveStrategy` (ذكي).
*   **كيف يعمل النظام الذكي؟** في دالة `update` داخل الـ `SmartAdaptiveStrategy` بنعمل حاجتين:
    1. بنشوف هل فيه أي طريق فيه عربية طوارئ (`hasPriorityVehicle`). لو فيه، بنفتح له فوراً.
    2. لو مفيش، بنشوف الطريق اللي فيه أكبر عدد سيارات (`maxVehicles`) وبنفتح له.
*   **ما دور الـ TrafficController؟** هو "الوسيط"؛ بياخد الـ Strategy ويطبقها على الـ Intersection. هو اللي بينادي `strategy->update()`.

---

<a name="kareem-section"></a>
## 👤 2. كريم: خبير الكائنات (Object Specialist)
**تركيزك الأساسي:** الوراثة (Inheritance) وأنواع العربيات.

### 🛠️ الملفات المسؤولة عنها:
- `Vehicle.h / .cpp`

### 💡 الأسئلة التي يجب أن تجيب عليها:
*   **اشرح الـ Inheritance في المشروع؟** عندنا كلاس أب `Vehicle` فيه الخصائص العامة (ID, Time). ورثنا منه `NormalVehicle` و `PriorityVehicle`. ومن الأخير ورثنا `Ambulance` و `PoliceCar`.
*   **ما هو الـ Abstract Class؟** هو كلاس `Vehicle`؛ لأنه يحتوي على دوال `= 0`. لا يمكن إنشاء Object منه مباشرة، وفائدته هو إجبار الأبناء على تعريف دوال معينة.
*   **كيف استخدمنا الـ Polymorphism؟** في دالة `isPriority()`. لما بنناديها على مؤشر من نوع `Vehicle*` بيعرف لوحده ينادي النسخة الخاصة بالإسعاف (ترجع true) أو العربية العادية (ترجع false).

---

<a name="mazen-section"></a>
## 👤 3. مازن: مسؤول البنية التحتية (Infrastructure Expert)
**تركيزك الأساسي:** كيف تترابط الطرق وكيف نحسب الإحصائيات.

### 🛠️ الملفات المسؤولة عنها:
- `Road.h / .cpp`
- `Intersection.h / .cpp`
- `TrafficLight.h / .cpp`
- `Statistics.h / .cpp`

### 💡 الأسئلة التي يجب أن تجيب عليها:
*   **كيف يتم تمثيل الطريق؟** الطريق عبارة عن `std::queue<Vehicle*>`؛ أول عربية دخلت هي أول عربية تخرج (FIFO).
*   **ما هو الـ Composition؟** الـ `Intersection` يتكون من (Has-a) مجموعة من الـ `Roads` والـ `TrafficLights`.
*   **كيف نحسب متوسط وقت الانتظار؟** في كلاس `Statistics`؛ كل ما عربية تعدي بنجمع وقت انتظارها في `totalWaitTime` ونقسمه على `totalVehiclesPassed` في الآخر.

---

<a name="mina-section"></a>
## 👤 4. مينا: مطور الواجهة والمحاكاة (UI Developer)
**تركيزك الأساسي:** إزاي المحاكاة بتشتغل كـ Loop وترسم على الشاشة.

### 🛠️ الملفات المسؤولة عنها:
- `main.cpp`
- `mainwindow.h / .cpp`

### 💡 الأسئلة التي يجب أن تجيب عليها:
*   **كيف تتحرك المحاكاة؟** بنستخدم `QTimer` بيشتغل كل 150 ملي ثانية. في كل "تكة" (Step)، بننادي دالة `updateSimulation`.
*   **ماذا يحدث في كل Step؟** 
    1. بنضيف عربيات عشوائية للطرق.
    2. بننادي الـ Controllers عشان يغيروا الإشارات.
    3. بنحرك العربيات اللي إشارتها خضراء ونحذفها من الـ Queue.
*   **كيف يتم الرسم؟** في دالة `paintEvent`؛ بنستخدم الـ `QPainter` لرسم المباني، الطرق، الإشارات، والعبريات حسب مكانها وحالتها.

---

<a name="download-section"></a>
## 📥 طريقة تحميل المشروع من GitHub (لأعضاء الفريق)

يا شباب، عشان تنزلوا المشروع عندكم، اتبعوا الخطوات دي بالظبط:

1.  افتح الـ **Terminal** أو الـ **CMD** في المكان اللي عايز تنزل فيه المشروع.
2.  اكتب الأمر ده:
    ```bash
    git clone https://github.com/ahmedanter2805/Smart-Traffic.git
    ```
3.  ادخل جوه الفولدر:
    ```bash
    cd Smart-Traffic
    ```
4.  افتح المشروع باستخدام **Qt Creator** عن طريق فتح ملف `CMakeLists.txt`.
5.  اعمل **Build** و **Run**.

---
**بالتوفيق يا شباب! لو حد وقف قدامه حاجة يكلمني فوراً.**

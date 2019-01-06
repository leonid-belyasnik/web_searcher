import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 1024
    height: 768
    title: qsTr("Web searcher")

    signal sigRun(int threadsNum, int maxDeep, string keyword, string rootUrl)
    signal sigStop()
    signal sigPause()

    property bool processing : false

    function setStatus(text) {
        if (processing)
        {
            status.text = text
        }
    }

    function onRunning() {
        status.text = "Running"
        processing = true
        progress.value = 0
        urlslist.resetList()
    }

    function onStopped() {
        setStatus("Stopped")
        processing = false
        progress.value = 0
    }

    function onResume() {
        status.text = "Running"
        processing = true
    }

    function onPaused() {
        setStatus("Paused")
        processing = false
    }

    function onDone() {
        status.text ="Done"
        processing = false
        progress.value = 100
    }

    function onProgress(num) {
        progress.value = num * 100 / maxDeep.value
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 3

        GroupBox {
            id: fieldsBox
            Layout.fillWidth: true

            RowLayout {
                id: rowFields
                anchors.fill: parent

                Label {
                    text: "Threads:"
                    font.bold : true
                }

                SpinBox {
                    id: threadsNum
                    objectName: "threadsNumSpin"
                }

                Label {
                    text: "Max deep:"
                    font.bold : true
                }

                SpinBox {
                    id: maxDeep
                    value: 50
                    objectName: "maxDeepSpin"
                }

                Label {
                    text: "Keyword:"
                    font.bold : true
                }

                TextField {
                    id: keyword
                    objectName: "keywordField"
                    placeholderText: "def.: 'test'"
                }

                TextField {
                    id: rootUrl
                    objectName: "rootUrlField"
                    placeholderText: "Root URL (def.: http://www.meter.net/ping-test/)"
                    Layout.fillWidth: true
                }
            }
        }

        GroupBox {
            id: buttonsBox
            Layout.fillWidth: true

            RowLayout {
                id: rowButtons
                anchors.fill: parent
                Button {
                    text: qsTr("Run")
                    onClicked: sigRun(threadsNum.value, maxDeep.value,
                                      (keyword.text.length == 0) ? "test" : keyword.text,
                                      (rootUrl.text.length == 0) ? "http://www.meter.net/ping-test/" : rootUrl.text)

                    Layout.fillWidth: true
                }
                Button {
                    text: qsTr("Stop")
                    onClicked: sigStop()
                }
                Button {
                    text: qsTr("Pause")
                    onClicked: sigPause()
                }
                Button {
                    text: qsTr("Quit")
                    onClicked: Qt.quit()
                }
            }
        }

        GroupBox {
            id: listBox
            Layout.fillWidth: true
            Layout.fillHeight: true

            UrlsList {
                id: urlslist
                anchors.fill: parent
            }
        }

    }

    ProgressBar {
        id: progress
        spacing: 15
        anchors.margins: 5
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        from: 0
        to: 100
    }

    footer: TextField {
        id: status
        text: ""
        Layout.fillWidth: true
        color: "green"
        readOnly: true
    }
}

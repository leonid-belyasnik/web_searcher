import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import Urls 1.0

ColumnLayout {
    spacing: 5
    anchors.fill: parent
    anchors.margins: 5

    function resetList() {
        listUrls.model.list = urlsList
        listUrls.update()
    }

    Frame {
        Layout.fillHeight: true
        Layout.fillWidth: true

        ListView {
            id: listUrls
            clip: true
            anchors.fill: parent

            headerPositioning: ListView.OverlayHeader

            header: Rectangle {
                id: headerItem
                height: 30

                Text {
                    text: "RESULT"
                    color: "green"
                }
            }

            ScrollBar.vertical: ScrollBar {}

            model: UrlsModel {
                list: urlsList

                onListChanged: {
                    console.log("reset list")
                }
            }

            delegate: RowLayout {
                width: parent.width
                TextField {
                    text: model.id
                    readOnly: true
                }
                TextField {
                    text: model.parent_id
                    readOnly: true
                }
                TextField {
                    text: model.url
                    readOnly: true
                    Layout.fillWidth: true
                }
                TextField {
                    text: model.state
                    readOnly: true
                }
            }
        }
    }
}

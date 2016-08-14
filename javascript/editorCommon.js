var modal;
var overlay = document.getElementById('modal-overlay');

/**
 * モーダルを開く
 */
function openModal(button){
    button.blur();
    switch (button.id) {
    case 'proj-Btn':
        modal = document.getElementById('new-proj');
        break;
    case 'tmpl-Btn':
        modal = document.getElementById('new-tmpl');
        break;
    }
    modal.style.visibility = 'visible';
    overlay.style.visibility = 'visible';
}

/**
 * モーダルを閉じる
 */
function hideModal() {
    // モーダルベースレイヤを非表示にする
    modal.style.visibility = 'hidden';
    overlay.style.visibility = 'hidden';
    modal = null;
}

overlay.addEventListener('click', hideModal,false);
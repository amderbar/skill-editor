var modal = document.getElementById('modal-content');
var overlay = document.getElementById('modal-overlay');

/**
 * モーダルを開く
 */
function openModal(button){
    button.blur();
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
}

overlay.addEventListener('click', hideModal,false);
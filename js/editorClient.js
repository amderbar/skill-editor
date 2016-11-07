/**
 * Editor on Browser: client script.
 */
$(function() {

/**
 * サイドメニューの表示、非表示切り替え関数
 */
$('#toggle_menu').click(function () {
	var side_menu = $('#side_menu', parent.document);
	if(side_menu.is(':visible')){
		$(this).attr('title', 'サイドメニューを開く');
	} else {
		$(this).attr('title', 'サイドメニューを閉じる');
	}
	side_menu.toggle('slide', 'linear', 100);
});


// タブ切り替え関数
function changeTab(tab) {
   // 現在のページを消す
   var current_tab = location.hash;
   if (current_tab) {
	   current_tab = current_tab.slice(current_tab.indexOf('#')+1);
	   document.getElementById(current_tab).style.display = 'none';
	   document.querySelector('.editor-tabs > li.editting').classList.toggle('editting');
   }
	// タブのクラス変更
	tab.parentNode.classList.toggle('editting');
   // 指定箇所のみ表示
   var tabname = tab.href;
   tabname = tabname.slice(tabname.indexOf('#')+1);
   document.getElementById(tabname).style.display = 'initial';
}


var dragSrcEl = null;

function handleDragStart(e) {
	// Target (this) element is the source node.
	e.dataTransfer.effectAllowed = 'move';
	e.dataTransfer.setData('text/html', this.innerHTML);
	dragSrcEl = this;
	this.classList.add('moving');
}

function handleDragOver(e) {
	if (e.preventDefault) {
		e.preventDefault(); // Necessary. Allows us to drop.
	}
	e.dataTransfer.dropEffect = 'move'; // See the section on the DataTransfer
	// object.
	return false;
}

function handleDragEnter(e) {
	// this / e.target is the current hover target.
	this.classList.add('over');
}

function handleDragLeave(e) {
	this.classList.remove('over'); // this / e.target is previous target
	// element.
}
function handleDrop(e) {
	// this/e.target is current target element.
	if (e.stopPropagation) {
		e.stopPropagation(); // Stops some browsers from redirecting.
	}
	// Don't do anything if dropping the same column we're dragging.
	if (dragSrcEl != this) {
		// Set the source column's HTML to the HTML of the columnwe dropped on.
		dragSrcEl.innerHTML = this.innerHTML;
		this.innerHTML = e.dataTransfer.getData('text/html');
		[].forEach.call(dragSrcEl.querySelectorAll('.editable'),
				function(elem) {
					elem.addEventListener('click', handleOnClick, false);
				});
		[].forEach.call(this.querySelectorAll('.editable'), function(elem) {
			elem.addEventListener('click', handleOnClick, false);
		});
		[].forEach.call(this.querySelectorAll('.copyBtn'), function(elem) {
			elem.addEventListener('click', handleCopy, false);
		});
	}
	return false;
}

function handleDragEnd(e) {
	// this/e.target is the source node.
	[].forEach.call(document.querySelectorAll('.draggable'), function(col) {
		col.classList.remove('moving');
	});
	[].forEach.call(document.querySelectorAll('.dropzone'), function(col) {
		col.classList.remove('over');
	});
}

// クリック時の動作
function handleOnClick(e) {
	var ancestor = findAncestor(this, "draggable");
	if (ancestor != null) {
		ancestor.setAttribute("draggable", "false");
	}
	if (!document.createElement)
		alert("要素作成失敗");
	if (this.classList.contains("textarea")) {
		var ele = document.createElement('textarea');
		ele.innerHTML = this.innerHTML;
		ele.setAttribute("cols", "40");
		ele.setAttribute("rows", "5");
	} else {
		var ele = document.createElement('input');
		ele.setAttribute("type", "text");
		ele.setAttribute("value", this.innerHTML);
	}
	ele.addEventListener('click', handleTextBox, false);
	ele.addEventListener('blur', handleOnBlur, false);
	ele.addEventListener("keypress", handleKeyPress, false);
	ele.addEventListener("keydown", handleKeyPress, false);
	this.innerHTML = "";
	this.classList.add("editting");
	this.appendChild(ele);
	ele.focus();
	ele.select();
}

function handleTextBox(e) {
	e.stopPropagation(); // Stops some browsers from redirecting.
}

// テキストボックスからフォーカスが外れた時
function handleOnBlur(e) {
	var str = document.createTextNode(this.value);
	this.parentNode.appendChild(str);
	var ancestor = findAncestor(this, "draggable");
	if (ancestor != null) {
		ancestor.setAttribute("draggable", "true");
	}
	this.parentNode.classList.remove('editting');
	this.parentNode.removeChild(this);
}

// 指定クラスを持つ直近の祖先要素を探す
function findAncestor(elem, target) {
	console.log(elem);
	var ancestor = elem.parentNode;
	if (!elem.parentNode.classList.contains(target)) {
		if (elem.parentNode.id == "editorArea") {
			ancestor = null;
		} else {
			ancestor = findAncestor(ancestor, target);
		}
	}
	return ancestor;
}

// テキストボックス編集関連
// キー操作
function handleKeyPress(e) {
	if ((e.which && e.which === 13) || (e.keyCode && e.keyCode === 13)) { // Enterが押された
		if (e.shiftKey) { // Shiftキーも押された
			// 何もしない(普通に改行)
		} else {
			e.preventDefault(); // ブラウザ標準の機能を停止
			this.blur();
		}
	} else if ((e.which && e.which === 9) || (e.keyCode && e.keyCode === 9)) { // Tabが押された
		var nextNode = findNext(this.parentNode, '.editable');
		if (nextNode != null) {
			e.preventDefault(); // ブラウザ標準の機能を停止
			// clickをエミュレート
			if ( /* @cc_on ! @ */false) { // IEの場合
				nextNode.fireEvent("onclick");
			} else { // Firefoxの場合
				var evt = document.createEvent("MouseEvents"); // マウスイベントを作成
				evt.initEvent("click", false, true); // イベントの詳細を設定
				// イベントを強制的に発生させる
				nextNode.dispatchEvent(evt);
			}
		}
	} else {
		// 何もしない(普通に入力)
	}
}

// 次のeditable要素を検索
function findNext(self, target) {
	var arr = document.querySelectorAll(target);
	var selflag = false;
	for (var i = 0, len = arr.length; i < len; ++i) {
		if (selflag) {
			return arr[i];
		}
		selflag = (arr[i] == self);
	}
	return null;
}

// 要素複製関数
function handleCopy(e) {
	var item = findAncestor(this, "draggable");
	var newitem = item.cloneNode(true);
	document.getElementById("editorArea").appendChild(newitem);
	putHandlers();
}

// ファイル保存関連
function handleDownload() {
	var content = document.getElementById("editorArea").innerHTML;
	var blob = new Blob([ content ], {"type" : "text/plain"});
	if (window.navigator.msSaveBlob) {
		window.navigator.msSaveBlob(blob, "save.txt");
		// msSaveOrOpenBlobの場合はファイルを保存せずに開ける
		window.navigator.msSaveOrOpenBlob(blob, "test.txt");
	} else {
		document.getElementById("download").href
			= window.URL.createObjectURL(blob);
	}
}

// ファイルをドロップして開く
// function handleFileDrop(e) {
// 	e.stopPropagation(); // Stops some browsers from redirecting.
// 	e.preventDefault();
// 	// files is a FileList of File objects. List some properties.
// 	fileLoder(e.dataTransfer.files);
// 	putHandlers();
// }
// ファイル選択フォームから選ぶ
function handleFileSelect(input) {
	console.log("onchange")
	console.log(input.value)
	var file_list = input.files;
		if(!file_list) return;
	fileLoder(file_list);
	putHandlers();
}
// ファイルを開く
function fileLoder (files) {
	var output = [];
	for (var i = 0, f; f = files[i]; i++) {
		output.push('<li><strong>', escape(f.name), '</strong>(', f.type || 'n/a', ')-',
				f.size, 'bytes,lastmodified:',
				f.lastModifiedDate ? f.lastModifiedDate.toLocaleDateString() : 'n/a', '</li>');
		// Only process text files.
		if (!f.type.match('text.*')) { continue; }
		var reader = new FileReader();
		// Closure to capture the file information.
		reader.onload = (function(theFile) { return function(e) {
				// Render contents.
				var innerDiv = document.createElement('div');
				innerDiv.innerHTML = e.target.result;
				document.getElementById('editorArea').appendChild(innerDiv);
				document.getElementById('editorArea').lastChild.onload = putHandlers();
			};})(f);
		// Read in the source file as a Text.
		reader.readAsText(f);
	}
	// document.getElementById('fileName').innerHTML = '<ul>' + output.join('') + '</ul>';	
}
function handleFileDragOver(evt) {
	evt.stopPropagation();
	evt.preventDefault();
	evt.dataTransfer.dropEffect = 'copy'; // Explicitly show this is a copy.
}

// イベントハンドラの設置関数。
function putHandlers() {
	// ドラッグアンドドロップ関連
	var editArea = document.getElementById('editorArea');
	[].forEach.call(editArea.getElementsByClassName('draggable'), function(col) {
		col.addEventListener('dragstart', handleDragStart, false);
		col.addEventListener('dragend', handleDragEnd, false);
	});
	[].forEach.call(editArea.getElementsByClassName('dropzone'), function(elem) {
		elem.addEventListener('drop', handleDrop, false);
		elem.addEventListener('dragenter', handleDragEnter, false)
		elem.addEventListener('dragover', handleDragOver, false);
		elem.addEventListener('dragleave', handleDragLeave, false);
	});
	// クリックして編集モード
	[].forEach.call(editArea.getElementsByClassName('editable'), function(elem) {
		elem.addEventListener('click', handleOnClick, false);
	});
	// 要素の複製
	[].forEach.call(editArea.getElementsByClassName('copyBtn'), function(elem) {
		elem.addEventListener('click', handleCopy, false);
	});
}
// 初期設定
putHandlers();
// ファイル保存関連
document.getElementById("download").addEventListener('click', handleDownload,false);
// ドラッグアンドドロップでファイルを開くイベントリスナー設置.
// document.getElementById('file_drop_zone').addEventListener('dragover',handleFileDragOver, false);
// document.getElementById('file_drop_zone').addEventListener('drop', handleFileDrop,false);
// document.getElementById('file_select').addEventListener('onchange', handleFileSelect,false);
});

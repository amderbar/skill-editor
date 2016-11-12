/**
 * テーブル作成時用JavaScript
 */
$(function() {
	/**
	 * カラム追加関数
	 */
	$('#add-col').click(function () {
		$(this).before($(this).prev().clone());
        var tbody = $('#def-tbl tbody');
		tbody.children('tr').each(function(i, elem){
			$(elem).append($(elem).children().last().clone());
		});
        // 初期化
        initColumn($('.col-h').length - 1, true);
        // 外部キー参照先選択セレクトボックスにも反映が必要
        // プレビューに反映
        $('#preview').find('.data-table').each(function (idx, tbl) {
            $(tbl).find('tr').each(function(jdx, row){
                var new_td = $(row).children().last().clone();
                if (new_td.prop("tagName") == 'TH') {
                    new_td.text('Nameless');
                }
                $(row).append(new_td);
            });
        });
		// イベントリスナー設置
		$('.col-h').unbind();
		$('.col-h').hover(function (e) { // ラベルの切り替え hover
				$(this).append($("<span>削除</span>"));
			},function (e) { // out
				$(this).find('span').last().remove();
		}).click(function (e) { // 列削除
            deleteCol(this);
		});
        putHandlers();
	});

	/**
	 * レイアウトプレビュー切り替え
	 */
	$('#tbl_templs').change(function () {
		var active = $(this).val();
		$('#preview').children().each(function (i, elem) {
			if ($(elem).is(':visible')) {
				$(elem).attr('hidden', true);
			} else if (i == active) {
				$(elem).attr('hidden', false);
			}
		});
	});

	/**
	 * イベントリスナー設置
	 */
	putHandlers();
});

/**
 * DOM操作ごとに行うイベントハンドラの設置関数
 */
function putHandlers() {
    // Enterでのフォーム送信を防止
    var inputs = $('input[class!=allow_submit]');
    inputs.unbind('keypress', preventSubmit);
    inputs.keypress(preventSubmit);

    // 表定義の入力形式の選択状態に応じて初期値入力フォームを変更したり追加の設定事項を表示したりする
    var tbody = $('#def-tbl tbody');
    tbody.find('select[name^="form_type"]').unbind('change', updateColumn);
    tbody.find('select[name^="form_type"]').change(updateColumn);

    // 表定義のフォーム変更をリアルタイムでプレビューに反映
    tbody.find('input').unbind('change', reflectPreview);
    tbody.find('input').change(reflectPreview);
}

/**
 * Enterでのフォーム送信を防止
 */
function preventSubmit(event) {
    return event.which !== 13;
}

/**
 * 列定義を初期化する関数
 */
function initColumn(col_num, complete) {
    var tbody = $('#def-tbl tbody');
    tbody.find('select[name^="form_type"]').eq(col_num).siblings('div').prop('hidden', true);
    tbody.find('input[name^="step"]').eq(col_num).val(1);
    tbody.find('input[name^="max"]').eq(col_num).val('');
    tbody.find('input[name^="min"]').eq(col_num).val('');
    var multi = tbody.find('input[name^="multi"][type="checkbox"]').eq(col_num).prop({disabled: false, checked: false});
    multi.siblings('input[type="hidden"]').remove();
    var not_null = tbody.find('input[name^="not_null"][type="checkbox"]').eq(col_num).prop('disabled', false);
    not_null.siblings('input[type="hidden"]').remove();
    if (complete) {
        tbody.find('input[name^="colname"]').eq(col_num).val('');
        tbody.find('input[name^="default"]').eq(col_num).attr('type', 'text').val('');
        tbody.find('input[name^="uniq"]').eq(col_num).prop('checked', false);
        not_null.prop('checked', false);
    }
}

/**
 * カラム削除関数
 */
function deleteCol(self) {
    var text = $(self).find('label').text();
    var index = $('.col-h').index(self);
    index++;
    if (confirm(text + index + "を削除してもよろしいですか？")) {
        $('#def-tbl tbody').children('tr').each(function(i, row){
            $(row).children().eq(index).remove();
        });
        $(self).remove();
        if ($('.col-h').length < 2) {
            $('.col-h').unbind();
        }
        // 外部キー参照先選択セレクトボックスにも反映が必要
        // プレビューに反映
        $('#preview').find('.data-table').each(function (idx, tbl) {
            $(tbl).find('tr').each(function(jdx, row){
                $(row).children().eq(index).remove();
            });
        });

    }
}

/**
 * 表定義の入力形式の選択状態に応じて初期値入力フォームを変更したり追加の設定事項を表示したりする
 */
function updateColumn(eve) {
    var tbody = $('#def-tbl tbody');
    var form_type = $(eve.target).val();
    var idx = tbody.find('select[name^="form_type"]').index(eve.target);
    tbody.find('input[name^="default"]').eq(idx).attr('type', form_type).val('');
    // いったん初期化
    initColumn(idx, false);
    // 変更の適用
    var not_null = tbody.find('input[name^="not_null"][type="checkbox"]').eq(idx);
    switch (form_type) {
        case 'range':
            $(eve.target).siblings('div[name="step"],div[name="max"],div[name="min"]').prop('hidden', false);
        case 'color':
            not_null.prop('checked', true);
            if (not_null.siblings('input[type="hidden"]').length < 1) {
                var hidden = not_null.clone();
                hidden.attr('type', 'hidden');
                not_null.prop('disabled', true).after(hidden);
            }
            break;
        case 'multicheck':
            var multi = $(eve.target).siblings('div[name="multi"]')
                .find('input[name^="multi"][type="checkbox"]').prop('checked', true);
            var hidden = multi.clone();
            hidden.attr('type', 'hidden');
            multi.prop('disabled', true).after(hidden);
        case 'selectbox':
            $(eve.target).siblings('div[name="multi"]').prop('hidden', false);
        case 'listext':
            $(eve.target).siblings('div[name="ref"]').prop('hidden', false);
            break;
    }
    reflectPreview(eve);
}

/**
 * 表定義のフォーム変更をリアルタイムでプレビューに反映
 */
function reflectPreview(eve) {
    var name = $(eve.target).attr('name');
        console.log($(eve.target));
    var index = $('#def-tbl tbody').find('input[name^=' + name.replace('[]', '') + ']').index(eve.target);
    var val = $(eve.target).val();
    switch (name) {
        case 'colname[]':
            // 外部キー参照先選択セレクトボックスにも反映が必要
            if (!val) {
                val = 'Nameless';
            }
            $('#preview').find('.data-table').each(function (idx, tbl) {
                $(tbl).find('tr').first().find('th').eq(index).text(val);
            });
            break;
    
        case 'type[]':
            break;
    
        case 'default[]':
            break;
    
        case 'uniq[]':
            break;
    
        case 'not_null[]':
            break;
    
        case 'foreign[]':
            break;
    
        case 'form_type[]':
            $('#preview').find('.data-table').each(function (idx, tbl) {
                $(tbl).find('tr').each(function(jdx, row){
                    $(row).children('input').eq(index).attr('type', val);
                });
            });
            break;
    }
}
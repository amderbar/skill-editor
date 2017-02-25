/**
 * テーブル作成時用JavaScript
 */
$(function() {
	/**
	 * カラム追加関数
	 */
	$('#add-col').click(function () {
        var tbody = $('#def-tbl tbody');
        var jdx = $('.col-h').length + 1;
		tbody.children('tr').each(function(idx, elem){
            var clone = $(elem).children().last().clone();
            var inputs = clone.find('input,select');
            inputs.each(function (k, input) {
                var name = $(input).attr('name');
                $(input).attr('name', name.replace(/\[\d+\]/, '[' + jdx + ']'));
            });
			$(elem).append(clone);
		});
		$(this).before($(this).prev().clone());
        // 初期化
        initColumn(jdx, true);
        // 外部キー参照先選択セレクトボックスに反映
        tbody.find('select[name*="ref_dist"]').each(function (idx, ref_dist) {
            $(ref_dist).find('optgroup').first().append($('<option/>', {value: -jdx}).text('Nameless' + jdx));
        });
        // プレビューに反映
        $('#preview .data-table').each(function (idx, tbl) {
            $(tbl).find('tr').each(function(jdx, row){
                var new_td = $(row).children().last().clone();
                if (new_td.prop("tagName") == 'TH') {
                    new_td.text('Nameless');
                }
                $(row).append(new_td);
            });
        });
		// イベントリスナー設置
		$('.col-h').off();
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
    inputs.off('keypress', preventSubmit);
    inputs.keypress(preventSubmit);

    // 表定義の入力形式の選択状態に応じて初期値入力フォームを変更したり追加の設定事項を表示したりする
    var tbody = $('#def-tbl tbody');
    tbody.find('select[name*="form_type"]').off('change', updateColumn);
    tbody.find('select[name*="form_type"]').change(updateColumn);

    // 表定義の変更をリアルタイムにプレビュー等影響下のフォームに反映
    tbody.find('input').off('change', updateForms);
    tbody.find('input').change(updateForms);
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
	col_num --;
    var tbody = $('#def-tbl tbody');
    tbody.find('select[name*="form_type"]').eq(col_num).siblings('div').prop('hidden', true);
    tbody.find('input[name*="step"]').eq(col_num).val(1).prop('disabled', true);
    tbody.find('input[name*="max"]').eq(col_num).val('').prop('disabled', true);
    tbody.find('input[name*="min"]').eq(col_num).val('').prop('disabled', true);
    tbody.find('select[name*="ref_dist"]').eq(col_num).val('').prop('disabled', true);
    var multiple = tbody.find('input[name*="multiple"][type="checkbox"]').eq(col_num).prop({disabled: true, checked: false});
    multiple.siblings('input[type="hidden"]').remove();
    var not_null = tbody.find('input[name*="not_null"][type="checkbox"]').eq(col_num).prop('disabled', false);
    not_null.siblings('input[type="hidden"]').remove();
    var default_input = tbody.find('[name*="default"]').eq(col_num);
    if (default_input.prop("tagName") != 'INPUT') {
        var from_name = default_input.attr('name');
        default_input.replaceWith($('<input>', {name: from_name}));
    }
    if (complete) {
        tbody.find('input[name*="col_name"]').eq(col_num).val('');
        tbody.find('input[name*="default"]').eq(col_num).attr('type', 'text').val('');
        tbody.find('input[name*="uniq"]').eq(col_num).prop('checked', false);
        not_null.prop('checked', false);
    }
}

/**
 * カラム削除関数
 */
function deleteCol(self) {
    var tbody = $('#def-tbl tbody');
    var text = $(self).find('label').text();
    var index = $('.col-h').index(self) + 1;
    if (confirm(text + index + "を削除してもよろしいですか？")) {
    	tbody.children('tr').each(function(idx, row){
            // name属性の添え字付け替え
            var cols = $(row).children();
            var new_jdx = index - 1;
            for (var jdx = index + 1; jdx < cols.length; jdx++) {
                $(cols[jdx]).find('input,select').each(function (kdx, input) {
                    var name = $(input).attr('name');
                    $(input).attr('name', name.replace(/\[\d+\]/, '[' + new_jdx + ']'));
                });
                new_jdx = jdx - 1;
            }
            // 削除実行
            cols.eq(index + 1).remove();
        });
        $(self).remove();
        if ($('.col-h').length < 2) {
            $('.col-h').off();
        }
        // 外部キー参照先選択セレクトボックスに反映
        tbody.find('select[name*="ref_dist"]').each(function (idx, ref_dist) {
            var options = $(ref_dist).find('optgroup').first().children('option');
            for (var jdx = index + 1; jdx < options.length; jdx++) {
                $(options[jdx]).val(-jdx + 1);
            }
            // 削除実行
            options.eq(index).remove();
        });
        // プレビューに反映
        $('#preview .data-table').each(function (idx, tbl) {
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
    var idx = tbody.find('select[name*="form_type"]').index(eve.target);
    // いったん初期化
    initColumn(idx + 1, false);
    // 変更の適用
    var form_type = $(eve.target).val();
    var default_input = tbody.find('[name*="default"]').eq(idx);
    if (/textarea|select/.test(form_type)) {
        var from_name = default_input.attr('name');
        default_input.replaceWith($('<' + form_type + '>', {name: from_name}));
    } else if (form_type == 'listext') {
        default_input.attr('type', 'text').val('');
    } else if (form_type == 'numlist') {
        default_input.attr('type', 'number').val('');
    } else if (/checkbox|multicheck/.test(form_type)) {
        default_input.attr('type', 'checkbox');
    } else if (form_type == 'radio') {
        default_input.attr('type', 'radio');
    } else {
        default_input.attr('type', form_type).val('');
    }
    var not_null = tbody.find('input[name*="not_null"][type="checkbox"]').eq(idx);
    // list属性の追加
    switch (form_type) {
        case 'listext':
        case 'numlist':
            default_input.attr('list', '');
            break;
    }
    // 外部参照先セレクトボックス表示
    switch (form_type) {
        case 'listext':
        case 'numlist':
        case 'select':
        case 'radio':
        case 'multicheck':
            $(eve.target).siblings('div[name="ref"]').prop('hidden', false)
            .find('input,select').prop('disabled', false);
            break;
    }
    // 刻み幅、最大値、最小値の入力欄表示
    switch (form_type) {
        case 'numlist':
        case 'number':
        case 'range':
            $(eve.target).siblings('div[name="step"],div[name="max"],div[name="min"]').prop('hidden', false)
            .find('input,select').prop('disabled', false);
            break;
    }
    // Not null 制約の強制
    switch (form_type) {
        case 'range':
        case 'color':
            not_null.prop('checked', true);
            if (not_null.siblings('input[type="hidden"]').length < 1) {
                var hidden = not_null.clone();
                hidden.attr('type', 'hidden');
                not_null.prop('disabled', true).after(hidden);
            }
            break;
    }
    // 複数選択使用チェックボックス表示
    switch (form_type) {
        case 'select':
        case 'multicheck':
            $(eve.target).siblings('div[name="multi"]').prop('hidden', false)
            .find('input,select').prop('disabled', false);
            break;
    }
    // 複数選択の強制
    switch (form_type) {
        case 'multicheck':
            var multiple = $(eve.target).siblings('div[name="multi"]')
                .find('input[name*="multiple"][type="checkbox"]').prop('checked', true);
            var hidden = multiple.clone();
            hidden.attr('type', 'hidden');
            multiple.prop('disabled', true).after(hidden);
            break;
    }
    // プレビューに反映
    updateForms(eve);
}

/**
 * 表定義の変更をリアルタイムにプレビュー等影響下のフォームに反映
 */
function updateForms(eve) {
    var name = $(eve.target).attr('name');
    name = name.match(/\[\d+\]\[(.+)\]/)[1];
    var tbody = $('#def-tbl tbody');
    var index = tbody.find('input[name*="' + name.replace(/\[\d\]/, '') + '"]').index(eve.target) + 1;
    var val = $(eve.target).val();
    switch (name) {
        case 'col_name':
            // 外部キー参照先選択セレクトボックスにも反映が必要
            if (!val) {
                val = 'Nameless' + (index + 1);
            }
            tbody.find('select[name*="ref_dist"]').each(function (idx, ref_dist) {
                    // TODO:出力文字列のエスケープ
                $(ref_dist).find('optgroup').first().children('[value="-' + index + '"]').text(val);
            });
            $('#preview .data-table').each(function (idx, tbl) {
                    // TODO:出力文字列のエスケープ
                $(tbl).find('tr').first().find('th').eq(index).text(val);
            });
            break;

        case 'form_type':
            $('#preview .data-table').each(function (idx, tbl) {
                $(tbl).find('tr').each(function(jdx, row){
                    $(row).children('input,select,textarea').eq(index).attr('type', val);
                });
            });
            break;

        case 'step':
        case 'max':
        case 'min':
            $('#preview .data-table').each(function (idx, tbl) {
                $(tbl).find('tr').each(function(jdx, row){
                    $(row).children('input,select,textarea').eq(index).attr(name, val);
                });
            });
            $('#def-tbl tbody [name*="default"]').eq(index).attr(name, val);
            break;

        case 'multiple':
            $('#preview .data-table').each(function (idx, tbl) {
                $(tbl).find('tr').each(function(jdx, row){
                    $(row).children('input,select,textarea').eq(index).prop('multiple', true);
                });
            });
            $('#def-tbl tbody [name*="default"]').eq(index).prop('multiple', true);
            break;

        case 'ref_dist':
            break;

        case 'default':
            break;

        case 'uniq':
            break;

        case 'not_null':
            break;

        case 'foreign':
            break;
    }
}
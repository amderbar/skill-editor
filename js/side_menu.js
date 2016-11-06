$(function(){
    /**
     * プロジェクト新規作成ボタン押下時の処理
     */
    $('#new-proj').click(function (e) {
        $('#menu-list').append($('<li/>')
            .append($('<div/>', {'class': 'with-btns'})
                .append($('<input type="text"/>').attr({'id': 'new-proj-name', 'value': 'untitled'}))
                .append($('<ul/>', {'class': 'btns'})
                    .append($('<li/>', {'class': 'icon-folder-plus btn', 'title': 'テーブルの追加'}))
                    .append($('<li/>', {'class': 'icon-bin btn', 'title': 'プロジェクトの削除'}))
                )
            )
        )
        var new_name_box = $('#new-proj-name');
        new_name_box.keypress(function (e) {
            if ( e.which == 13 ) {
                $(this).blur();
            }
        });
        new_name_box.blur(function (){
            var proj_name = $(this).val();
            $(this).replaceWith($('<h2>', {'text': proj_name}));
            $.post("index.php",
                { 'proj-name': proj_name }
            ).done(function(id){
                    location.reload(true);
                    changeEditor(id);
            }).fail(function(data){
                alert('error!!!');
            });
        }).focus();
    });

    /**
     * 疑似submitボタン押下時の処理
     */
    $('.submit').click(function(eve) {
        var proj_id = $(this).closest('form').find('input[name="id"]').val();
        var f_mode = $(this).find('input[name="fMode"]').val();
        switch (f_mode) {
            case 'del-prj':
                /* プロジェクト削除ボタンの時。確認ダイアログ表示 */
                var proj_name = $(this).closest('.with-btns').find('h2').text();
                console.log(location.pathname);
                if (confirm("プロジェクト「" + proj_name + "」を削除してもよろしいですか？")) {
                    $.post(location.pathname,
                        { 'id': proj_id, 'fMode': f_mode }
                    ).done(function(){
                            location.reload(true);
                            parent.editor_area.location.reload(true);
                    }).fail(function(data){
                        console.log(data);
                        alert('error!!!');
                    });
                }
                break;
            default:
                return true;
        }
    });
});

function changeEditor(id) {
    var query_str = (id != null) ? '?id=' + id : '';
    parent.editor_area.location.href = 'editor_area.php' + query_str;
}

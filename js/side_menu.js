$(function(){
    $('#new-proj').click(function (e) {
        $('#menu-list').append($('<li/>')
            .append($('<div/>', {'class': 'with-btns'})
                .append($('<input type="text"/>').attr({'id': 'new-proj-name', 'value': 'untitled'}))
                .append($('<ul/>')
                    .append($('<li/>', {'class': 'icon-folder-plus btn', 'title': 'テーブルの追加'}))
                    .append($('<li/>', {'class': 'icon-bin btn', 'title': 'プロジェクトの削除'}))
                )
            )
        )
        $('#new-proj-name').blur(function(){
            var proj_name = $(this).val();
            $(this).replaceWith($('<h2>', {'text': proj_name}));
            $.post("index.php",
                { 'proj-name': proj_name }
            ).done(function(id){
                    alert("new proj id: " + id);
                    changeEditor(id);
                    location.reload(true);
            }).fail(function(data){
                alert('error!!!');
            });
        }).focus();
    });
});

function changeEditor(id) {
    parent.editor_area.location.href = "editor_area.php?id=" + id;
}

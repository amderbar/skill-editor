<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title><?=APP_NAME?></title>

    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/main_panel.css')?>">
    <?php /* TODO:出来上がったら圧縮版を読み込むように変更する */ ?>
    <script src="https://unpkg.com/vue"></script>
    <script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
</head>
<body>

    <nav id="side_menu" v-show="nav_show">

        <header class="with-btns">
            <h2>Projects</h2>
            <ul class="btns">
                <li title="新規作成"><a href="#" id="new-proj" class="icon-database btn" @click="create_proj = true"></a></li>
            </ul>
        </header>

        <ul class="side-menu" id="proj-list">
            <?php foreach (($proj_list ?? []) as ['id' => $proj_id, 'proj_name' => $proj_name]) { ?>
                <li>
                <div class="with-btns">
                    <h2><?=Html::link($proj_name, $URL, ['pid' => $proj_id], ['onclick' => 'changeEditor('.$proj_id.')'])?></h2>
                    <?=Html::startForm(APP_ROOT)?>
                        <?=Html::hidden('pid', $proj_id)?>
                        <ul class="btns">
                            <li class="icon-folder-plus btn submit" title="テーブルの追加" @click="addTbl"></li>
                            <li class="icon-bin btn submit" title="プロジェクトの削除" @click="deleteProj"></li>
                        </ul>
                    <?=Html::endForm()?>
                </div>
                <?php if (isset($focus_pid) && $proj_id == $focus_pid) { ?>
                    <ul class="side-children">
                        <?php foreach ( ($tbl_list ?? []) as ['id' => $tbl_id, 'tbl_name' => $tbl_name] ) { ?>
                            <li class="with-btns">
                                <?=Html::link($tbl_name, '#', [], ['onclick' => 'changeEditor('.$proj_id.','.$tbl_id.')'])?>
                                <ul class="btns"><li class="icon-bin btn" title="テーブルの削除"></li></ul>
                            </li>
                        <?php } ?>
                    </ul>
                <?php } ?>
                </li>
            <?php } ?>
            <new-proj-form v-if="create_proj" @cancel="create_proj = false" v-cloak></new-proj-form>
            <!-- <li><input type="file" id="file_select" onchange="handleFileSelect(this)"></li>-->
        </ul>

    </nav>

    <button type="button" title="サイドメニューを閉じる" id="toggle-menu" @click="navToggle" v-cloak>{{ content }}</button>

    <section>
        <header id="tab-bar">
            <ul class="editor-tabs">
                <?php foreach ( ($tbl_list ?? []) as ['id' => $tbl_id, 'tbl_name' => $tbl_name] ) { ?>
                    <?php $tab_class = (isset($focus_tab) && $tbl_id === $focus_tab ) ? ' class="editting"' : '';?>
                    <li <?=$tab_class?>><?=Html::link($tbl_name, $URL, ['pid' => $proj_id, 'tab' => $tbl_id])?></li>
                <?php  } ?>
            </ul>
        </header>
        <?php /* <iframe name="editor_area" src="editor<?= isset($proj_id) ? '?pid='.$proj_id : '' ?>" scrolling="no" frameborder="no"></iframe> */ ?>
    </section>

    <script type="text/x-template" id="new-proj-template">
        <li><div><?=Html::startForm(APP_ROOT . '/create', null, 'POST', ['id' => 'new-proj-form'])?>
            <label><h2><?=Html::textbox('proj_name', '', [
                    'id' => 'new-proj-name',
                    'placeholder' => '新しいプロジェクトの名前',
                    'required' => true,
                    'v-model' => 'proj_name',
                    '@blur' => 'submit',
                    '@keydown.esc' => 'cancel'
            ])?></h2></label>
        <?=Html::endForm()?></div></li>
    </script>

    <script type="text/javascript">
        (function (parent) { return new Vue({
            el: '#toggle-menu',
            parent: parent,
            computed: {
                content: function(){
                    if(this.$parent.nav_show){
                        return '<<';
                    } else {
                        return '>>';
                    }
                }
            },
            methods: {
                navToggle: function() {
                    this.$parent.$emit('toggle');
                }
            }
        });})(new Vue({
            el: '#side_menu',
            created: function () {
                this.$on('toggle', function(){ this.nav_show = !this.nav_show; })
            },
            data: {
                nav_show: true,
                create_proj : false
            },
            methods: {
                addTbl: function(event) {
                    var proj_id = $(event.target).closest('form').find('input[name="pid"]').val();
                    console.log('addTble:' + proj_id);
                },
                deleteProj: function(event) {
                    var $form = $(event.target).closest('form');
                    var proj_name = $form.prev().text();
                    if (confirm("プロジェクト「" + proj_name + "」を削除してもよろしいですか？")) {
                        var action = $form.attr('action');
                        $form.attr('action', action + '/delete').submit();
                    }
                }
            },
            components: {
                'new-proj-form': {
                    template: '#new-proj-template',
                    data: function(){
                        return {
                            proj_name: 'untitled'
                        };
                    },
                    mounted: function() {
                        $(this.$el).find('#new-proj-name').focus();
                    },
                    methods: {
                        submit: function () {
                            $('#new-proj-form').submit();
                        },
                        cancel: function () {
                            this.$emit('cancel');
                        }
                    }
                }
            }
        }));

//     function changeEditor(pid, tab) {
//         var return_val = true;
//         if (pid != null) {
//             var query_str = '?pid=' + pid;
//             if (tab != null) {
//                 query_str += '&tab=' + tab;
//                 return_val = false;
//             }
//             parent.editor_area.location.href = 'editor_area.php' + query_str;
//         } else {
//             parent.editor_area.location.reload(true);
//         }
//         return return_val;
//     }
    </script>
</body>
</html>
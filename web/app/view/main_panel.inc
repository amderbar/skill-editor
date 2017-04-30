<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/common.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/fonts.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/main_panel.css')?>">
    <?php /* TODO:出来上がったら圧縮版を読み込むように変更する */ ?>
    <script src="https://unpkg.com/vue"></script>
    <script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
    <script type="text/javascript">
        $(function() {
            <?php /*Ctrl + S押下時のデフォルト動作を抑制*/ ?>
            $(window).keydown(function(e) {
                if ((e.which == '115' || e.which == '83' ) && (e.ctrlKey || e.metaKey)) {
                    e.preventDefault();
                    return false;
                }
                return true;
            });
        });
    </script>
    <title><?=APP_NAME?></title>
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
                    <h2><?=Html::link($proj_name, APP_ROOT . '/main', ['pid' => $proj_id])?></h2>
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
                            <li><span class="with-btns">
                                    <?=Html::link(
                                        $tbl_name,
                                        APP_ROOT . "/editor/data/{$focus_pid}",
                                        ['tab' => $tbl_id],
                                        ['target' => 'editor_area', '@click' => "changeFocus({$tbl_id})"])?>
                                    <ul class="btns"><li class="icon-bin btn" title="テーブルの削除"></li></ul>
                            </span></li>
                        <?php } ?>
	                    <li v-if="add_tbl" v-cloak><strong>untitled</strong></li>
                    </ul>
                <?php } ?>
                </li>
            <?php } ?>
            <new-proj-form v-if="create_proj" @cancel="create_proj = false" v-cloak></new-proj-form>
            <!-- <li><input type="file" id="file_select" onchange="handleFileSelect(this)"></li>-->
        </ul>

    </nav>

    <button type="button" :title="title" id="toggle-menu" @click="navToggle" v-cloak>{{ content }}</button>

    <main id="main-panel">
        <section>
            <header id="tab-bar">
                <ul class="editor-tabs">
                    <?php foreach ( ($tbl_list ?? []) as ['id' => $tbl_id, 'tbl_name' => $tbl_name] ) { ?>
                        <li :class="{editting: focus_tbl == <?=$tbl_id?>}" @click="focus_tbl = <?=$tbl_id?>">
                            <?=Html::link($tbl_name, APP_ROOT . "/editor/data/{$focus_pid}", ['tab' => $tbl_id], ['target' => 'editor_area'])?>
                        </li>
                    <?php  } ?>
                    <li class="editting" v-if="add_tbl" v-cloak><strong>untitled</strong></li>
                </ul>
            </header>
            <iframe class="tab-page" name="editor_area" v-bind:src="editor_focus" scrolling="no" frameborder="no"></iframe>
        </section>
    </main>

    <script type="text/x-template" id="new-proj-template">
        <li><div><?=Html::startForm('create', null, 'POST', ['id' => 'new-proj-form'])?>
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

        var vm_main_panel = new Vue({
            el: '#main-panel',
            created: function (pid) {
                this.$on('add-tbl', function(pid){
                    this.func = 'table/'
                    this.focus_pid = pid;
                    this.focus_tbl = null;
                });
            },
            data: {
                func: '<?=count($tbl_list ?? []) ? 'data/' : 'table/'?>',
                focus_pid: <?=Html::escape($focus_pid ?? 'null')?>,
                focus_tbl: <?=Html::escape($focus_tbl ?? 'null')?>
            },
            computed: {
                editor_focus: function () {
                    var query = '';
                    if (this.focus_pid) {
                        query += this.focus_pid;
                        if (this.focus_tbl) {
                            query += '?tab=' + this.focus_tbl;
                        }
                    }
                    return '<?=APP_ROOT?>/editor/' + this.func + query;
                },
                add_tbl: function () { return this.func == 'table/'; }
            }
        });

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
                },
                title: function(){
                    if(this.$parent.nav_show){
                        return 'サイドメニューを閉じる';
                    } else {
                        return 'サイドメニューを開く';
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
            parent: vm_main_panel,
            created: function () {
                this.$on('toggle', function(){ this.nav_show = !this.nav_show; });
            },
            data: {
                nav_show: true,
                create_proj : false,
            },
            computed: {
                add_tbl: function () { return this.$parent.add_tbl; }
            },
            methods: {
                addTbl: function(event) {
                    var proj_id = $(event.target).closest('form').find('input[name="pid"]').val();
                    this.$parent.$emit('add-tbl', proj_id);
                },
                deleteProj: function(event) {
                    var $form = $(event.target).closest('form');
                    var proj_name = $form.prev().text();
                    if (confirm("プロジェクト「" + proj_name + "」を削除してもよろしいですか？")) {
                        var action = $form.attr('action');
                        $form.attr('action', action + '/delete').submit();
                    }
                },
                changeFocus: function (tab_id) { this.$parent.focus_tbl = tab_id; }
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
    </script>
</body>
</html>
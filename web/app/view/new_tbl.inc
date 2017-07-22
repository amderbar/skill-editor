<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/common.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/fonts.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/editor_area.css')?>">
    <script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
    <?php /* TODO:出来上がったら圧縮版を読み込むように変更する */ ?>
    <script src="https://unpkg.com/vue"></script>
    <script type="text/javascript">
    <!--//<?php/*エディタ部共通のJavaScript*/?>
        $(function () {
            /**
             * フォームの変更を検知して画面遷移時の確認ダイアログを出すようにする
             */
            $('#table-form').change(function (){
                window.onbeforeunload = function (e) {
                    return 'Are you sure?';
                };
            });
        });
    //-->
    </script>
    <script src="<?=addFilemtime(APP_ROOT . '/js/common.js')?>"></script>
    <title><?=APP_NAME?></title>
</head>
<body>
    <header>
        <h1>New Table <?=Html::textbox('tbl_name', 'untitled', [
                'required' => true,
                'autofocus' => true,
                'form' => 'table-form'
            ]);?> @ <?=Html::escape($proj_name);?></h1>
    </header>
    <main id="main">
    <h2>Table Data definition</h2>
    <?=Html::startForm(APP_ROOT . "/editor/table/register", null, 'POST', ['id' => 'table-form', 'target' => '_parent'])?>
    <fieldset name="def-cols">
    <legend>Column definitions</legend>
    <?=Html::hidden('pid', $proj_id);?>
    <table class="data-table" id="def-tbl">
        <thead>
            <tr>
                <th></th>
                <th>No列</th>
                <th is="def-h" v-for="(col, id) in column_config"
                    v-if="col.key != 'id'"
                    :key="id"
                    :is_only="column_config.length < 3"
                    @click="dropColumn(id, $event)"></th>
                <th id="add-col" rowspan="6">
                    <button type="button" title="列追加" id="add-column" @click="addColumn">+</button>
                </th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <th>列名</th>
                <td>No.</td>
                <td v-for="(col, id) in column_config" v-if="col.key != 'id'" :key="id">
                    <?=Html::textbox('', '', [
                        'required' => true,
                        ':name' => "'def_cols[' + id + '][col_name]'",
                        'v-model' => 'column_config[id].col_name'
                    ]);?>
                </td>
            </tr>
            <tr>
                <th>入力形式</th><td>--</td>
                <td v-for="(col, id) in column_config" v-if="col.key != 'id'" :key="id">
                    <?=Html::selectbox('', function ($options_builder) use ($form_types) {
                        foreach ($form_types as $type => $label) {
                            $options_builder->option($type, $label);
                        }
                    }, [], false, [
                        ':name' => "'def_cols[' + id + '][form_type]'",
                        'v-model' => 'column_config[id].type'
                    ]);?>
                    <div name="step" v-if="isNumeric[id]" v-cloak>
                        <label>刻み幅:
                            <?=Html::number('', 1, null, [
                                ':disabled' => '!isNumeric[id]',
                                ':name' => "'def_cols[' + id + '][step]'",
                                'v-model' => 'column_config[id].step'
                            ]);?>
                        </label>
                    </div>
                    <div name="max" v-if="isNumeric[id]" v-cloak>
                        <label>最大値:
                            <?=Html::number('', null, null, [
                                ':disabled' => '!isNumeric[id]',
                                ':name' => "'def_cols[' + id + '][max]'",
                                'v-model' => 'column_config[id].max'
                            ]);?>
                        </label>
                    </div>
                    <div name="min" v-if="isNumeric[id]" v-cloak>
                        <label>最小値:
                            <?=Html::number('', null, null, [
                                ':disabled' => '!isNumeric[id]',
                                ':name' => "'def_cols[' + id + '][min]'",
                                'v-model' => 'column_config[id].min'
                            ]);?>
                        </label>
                    </div>
                    <div name="multi" v-if="isMultipleEnable[id] || isMustMultiple[id]" v-cloak>
                        <label>
                            <?=Html::radiocheck('checkbox', '', [1 => null], [], [
                                ':disabled' => '!isMultipleEnable[id] || isMustMultiple[id]',
                                ':name' => "'def_cols[' + id + '][multiple]'",
                                'v-model' => 'column_config[id].multiple'
                            ]);?>
                            <?=Html::hidden('', true, [
                                ':name' => "'def_cols[' + id + '][multiple]'",
                                'v-if' => 'isMustMultiple[id]'
                            ]);?>
                            複数選択をする
                        </label>
                    </div>
                    <div name="ref" v-if="isForginEnable[id]" v-cloak>
                        <label>選択肢<br v-if="isExistRefDest">
                            <?=Html::selectbox('', function ($options_builder) use($col_list) {
                                $options_builder->optgroup('This table', function ($options_builder) {
                                    $options_builder->option('', '{{ ref.col_name || \'列\' + jd }}', [
                                        'v-for' => '(ref, jd) in column_config',
                                        'v-if'  => "!['id', col.key].some(function (elm){ return elm == ref.key; })",
                                        ':key'  => 'ref.key',
                                        ':value'=> '- jd'
                                    ]);
                                }, ['v-if' => 'column_config.length > 2']);
                                foreach ($col_list as ['tbl_name' => $tblname, 'columns' => $colmns]) {
                                    $options_builder->optgroup($tblname, function ($options_builder) use($colmns) {
                                        foreach ($columns as $col_id => $col_name) {
                                            $options_builder->option($col_id, $col_name);
                                        }
                                    });
                                }
                            }, [], false, [
                                ':disabled' => '!isForginEnable[id]',
                                ':name' => "'def_cols[' + id + '][ref_dist]'",
                                'v-if' => 'isExistRefDest',
                                'v-model' => 'column_config[id].ref_dist'
                            ]);?>
                            <span v-if="!isExistRefDest">:候補がありません。</span>
                        </span>
                    </div>
                </td>
            </tr><tr><th>初期値</th><td>自動</td>
                <td v-for="(col, id) in column_config" v-if="col.key != 'id'" :key="id">
                    <?=Html::textbox('', '', [
                        ':name' => "'def_cols[' + id + '][default]'",
                        'v-model' => 'column_config[id].default'
                    ]);?>
                </td>
            </tr><tr><th>一意</th>
                <td><?=Html::radiocheck('checkbox', '', [1 => null], [1], ['disabled' => true]);?></td>
                <td v-for="(col, id) in column_config" v-if="col.key != 'id'" :key="id">
                    <?=Html::radiocheck('checkbox', '', [1 => null], [], [
                        ':name' => "'def_cols[' + id + '][uniq]'",
                        'v-model' => 'column_config[id].uniq'
                    ]);?>
                </td>
            </tr><tr><th>非Null</th>
                <td><?=Html::radiocheck('checkbox', '', [1 => null], [1], ['disabled' => true]);?></td>
                <td v-for="(col, id) in column_config" v-if="col.key != 'id'" :key="id">
                    <?=Html::radiocheck('checkbox', '', [1 => null], [], [
                        ':disabled' => 'isMustNotNull[id]',
                        ':name' => "'def_cols[' + id + '][not_null]'",
                        'v-model' => 'column_config[id].not_null'
                    ]);?>
                    <?=Html::hidden('', true, [
                        ':name' => "'def_cols[' + id + '][not_null]'",
                        'v-if' => 'isMustNotNull[id]'
                    ]);?>
                </td>
            </tr>
        </tbody>
    </table>
    </fieldset>
    <?=Html::submit('save', '作成');?>
    <?=Html::endForm()?>

    <h2>Table Desgin Preview : Template
        <?=Html::selectbox('tbl_templs', function ($options_builder) use($tbl_tmpls) {
            foreach (array_keys($tbl_tmpls) as $tmpl_name) {
                $options_builder->option($tmpl_name, $tmpl_name);
            }
        }, [], false, [
            'form' => 'table-form',
            'id' => 'tbl_templs',
            'v-model' => 'selected_template'
        ]);?>
    </h2>
    <div id="preview">
    <?php /* プレビュー表示用テンプレート部品読み込み */?>
    <?php foreach ($tbl_tmpls as $tmpl_name => $path) { ?>
        <div v-show="selected_template == '<?=Html::escape($tmpl_name);?>'" v-cloak>
            <?php View::include($path, ['REQ_SCOPE' => $REQ_SCOPE, 'tmpl_name' => $tmpl_name]); ?>
        </div>
    <?php } ?>
    </div>
    </main>
</body>
<?php
    /* プレビュー表示用JavaScriptの読み込み */
    foreach (array_keys($tbl_tmpls) as $tmpl_name) {
        View::yield('script@' . $tmpl_name);
    }
    View::include(full_path(VIEW_ROOT . '/vue_components.inc'));
?>
<?php /* 列定義編集テーブルのヘッダセルに関するコンポーネント */?>
<script type="text/x-template" id="def_tbl-header-template">
    <th class="col-h" @click="emitClick($event)" @mouseenter="on_hover = true" @mouseleave="on_hover = false">
        <slot></slot>
        <label>列</label>
        <span v-show="on_hover && !is_only">削除</span>
    </th>
</script><script type="text/javascript">
    Vue.component('def-h', {
        template: '#def_tbl-header-template',
        props: ['is_only'],
        data: function () { return {
            on_hover: false
        }},
        methods: {
            emitClick: function (event) {
                if (!this.is_only) {
                    this.$emit('click', event);
                }
            }
        }
    });

    <?php /* 画面全体の動作を司るVueインスタンス */ ?>
    var test = new Vue({
        el: '#main',
        data: {
            column_config: <?=json_encode($tbl_data['column_config'])?>,
            tbl_data: <?=json_encode($tbl_data['data'])?>,
            modified_form: [],
            selected_template: '<?=Html::escape(key($tbl_tmpls))?>',
            current_key: 1
        },
        computed: {
            default_data: function () {
                var default_data = {};
                this.column_config.forEach(function (conf) {
                    default_data[conf.key] = conf.default;
                }, this);
                return default_data;
            },
            isExistRefDest: function () {
                return (<?=Html::escape(array_sum(array_map(function ($col) {
                    return count($col['columns']);
                }, $col_list)))?> > 0) || (this.column_config.length > 2);
            },
            isNumeric: function () {
                return this.column_config.map(function (conf) {
                    switch (conf.type) {
                        case 'numlist':
                        case 'number':
                        case 'range':
                            return true;
                        default:
                            return false;
                    }
                });
            },
            isMultipleEnable: function () {
                return this.column_config.map(function (conf) {
                    switch (conf.type) {
                        case 'select':
                        case 'multicheck':
                            return true;
                        default:
                            return false;
                    }
                });
            },
            isForginEnable: function () {
                return this.column_config.map(function (conf) {
                    switch (conf.type) {
                        case 'listext':
                        case 'numlist':
                        case 'select':
                        case 'radio':
                        case 'multicheck':
                            return true;
                        default:
                            return false;
                    }
                });
            },
            isMustMultiple: function () {
                return this.column_config.map(function (conf) {
                    switch (conf.type) {
                        case 'multicheck':
                            conf.multiple = true;
                            return true;
                        default:
                            return false;
                    }
                }.bind(this));
            },
            isMustNotNull: function () {
                return this.column_config.map(function (conf) {
                    switch (conf.type) {
                        case 'range':
                        case 'color':
                            conf.not_null = true;
                            return true;
                        default:
                            return false;
                    }
                }.bind(this));
            }
        },
        methods: {
            change: function (new_val, key_name, row_id) {
                Vue.set(this.tbl_data[row_id], key_name, new_val);
                var form_name = key_name + '[' + row_id + ']';
            },
            push: function (new_row) {
                var row_id = this.tbl_data.length;
                this.tbl_data.push(new_row);
            },
            addColumn: function () {
                var newKey = (function generateNewKey() {
                    var trialKey = 'c' + Math.random().toString(36).slice(-8);
                    return this.column_config.some(function (col){
                        return col.key == trialKey;
                    }) ? generateNewKey() : trialKey;
                }.bind(this))();
                this.column_config.push($.extend(<?=json_encode(end($tbl_data['column_config']))?>, {key: newKey}));
                this.tbl_data = this.tbl_data.map(function (row, id, data) {
                    return $.extend(row, {[newKey]: (id != data.length - 1) ? '' : null});
                }, this);
            },
            dropColumn: function (key, event) {
                var eve_target = event.currentTarget || event.target;
                var text = $(eve_target).find('label').text();
                var index = $('.col-h').index(eve_target) + 1;
                if (confirm(text + index + "を削除してもよろしいですか？")) {
                    this.column_config.splice(key, 1);
                }
            }
        }
    });

</script>
</html>

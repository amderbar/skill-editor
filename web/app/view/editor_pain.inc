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
    <?php /*エディタ部共通のJavaScript*/ ?>
        $(function() {
            <?php /* フォームの変更を検知して画面遷移時の確認ダイアログを出すようにする */ ?>
            $('#table-form').change(function(){
                window.onbeforeunload = function(e) {
                    return 'Are you sure?';
                };
            });
            <?php /*ctrl+s押下時にform送信(上書き保存)*/ ?>
            $(window).keydown(function(e) {
                if ((e.which == '115' || e.which == '83' ) && (e.ctrlKey || e.metaKey)) {
                    e.preventDefault();
                    window.onbeforeunload = null;
                    $('#table-form').submit();
                }
                return true;
            });
        });
    </script>
    <title><?=APP_NAME?></title>
</head>
<body>
    <?=Html::startForm(APP_ROOT . '/editor/data/modify', null, 'POST', ['id' => 'table-form']) ?>
    <?=Html::hidden('pid', $proj_id) ?>
    <?=Html::hidden('tab', $tab_id) ?>

    <?php /*######## 表示テンプレートに置き換え ｺｺｶﾗ ########*/?>
    <table class="data-table">
        <thead><tr>
            <th v-for="(col_name, key_name) in column_names" v-bind:key="key_name">{{ col_name }}</th>
        </tr></thead>
        <tbody>
            <tr is="data-row" v-for="(row, id) in tbl_data"
                :key="id"
                :row_id="id"
                :row_data="row"
                :form_config="form_config"
                :modified_form="modified_form"
                @change="change">
            </tr>
            <tr is="data-row" id="new-rec"
                :row_id="null"
                :row_data="default_data"
                :form_config="form_config"
                @push="push">
            </tr>
        </tbody>
    </table>
    <?php /*######## 表示テンプレートに置き換え ｺｺﾏﾃﾞ ########*/?>

    <?=Html::endForm()?>
</body>

<?php /* データテーブルの行に関するコンポーネント */?>
<script type="text/x-template" id="data-row-template">
    <tr>
        <data-cell v-for="(cell, key_name) in row"
                :key="key_name"
                :row_id="row_id"
                :key_name="key_name"
                :cell="cell"
                :form_config="form_config"
                :modified_form="modified_form"
                @change="emitChange"
                @push="emitPush">
        </data-cell>
    </tr>
</script><script type="text/javascript">
    Vue.component('data-row', {
        template: '#data-row-template',
        props: ['row_data', 'row_id', 'form_config', 'modified_form'],
        data: function () { return {
            row: $.extend({}, this.row_data),
            modified: false,
            focus_key: null
        }},
        methods: {
            emitChange: function (new_val, key_name) {
                Vue.set(this.row, key_name, new_val);
                if(this.row_id != null) {
                    this.$emit('change', new_val, key_name, this.row_id);
                }
            },
            emitPush: function (new_val, key_name) {
                this.$emit('push', $.extend(this.row, {[key_name]: new_val}));
                if(this.row_id == null) {
                    this.row = $.extend({}, this.row_data);
                }
            }
        }
    });
</script>

<?php /* データテーブルのセルに関するコンポーネント */?>
<script type="text/x-template" id="data-cell-template">
    <thd v-bind:tag="tag_name" @click="focus">
            <input class="data-form"
                    :type="form_type"
                    :name="form_name"
                    :value="cell"
                    :readonly="!focused"
                    :disabled="!modified && !focused && key_name != 'id'"
                    @change="emitChange($event.target.value)"
                    @keydown.enter.prev="shiftRow"
                    @keydown.tab.stop="shiftColumn"
                    @keydown.esc="blur"
                    @blur="blur">
    </thd>
</script><script type="text/javascript">
    Vue.component('data-cell', {
        template: '#data-cell-template',
        props: ['row_id', 'key_name', 'cell', 'form_config', 'modified_form'],
        data: function () { return {
            focused: false
        }},
        computed: {
            tag_name: function () { return (this.key_name == 'id') ? 'h' : 'd'; },
            form_type: function () { return this.form_config[this.key_name].type; },
            form_name: function () { return this.key_name + '[' + this.row_id + ']'; },
            modified: function () { return ((this.modified_form || []).indexOf(this.form_name) >= 0); }
        },
        methods: {
            focus: function (event) {
                this.focused = true;
                var target = event.currentTarget || event.target;
                var form_name = this.form_name;
                Vue.nextTick(function () {
                    setTimeout(function () {
                        $(target).children('.data-form').focus();
                    }, 10);
                });
            },
            blur: function (event) {
                this.focused = false;
            },
            shiftColumn: function (event) {
                var shift_step = event.shiftKey ? -1 : 1;
                var idx = $('.editable').index($(event.target).closest('.editable'));
                $('.editable').eq(idx + shift_step).click();
            },
            shiftRow: function (event) {
                var shift_step = event.shiftKey ? -1 : 1;
                var selector = '[name^="' + this.key_name + '"]';
                var idx = $(selector).index($(event.target).closest('.editable').find(selector));
                if(!event.shiftKey && this.row_id == null) {
                    this.emitPush(event.target.value);
                    event.target.value = this.cell;
                } else {
                    Vue.nextTick(function () {
                        $(selector).eq(idx + shift_step).closest('.editable').click();
                    });
                }
            },
            emitChange: function (new_val) {
                this.$emit('change', new_val, this.key_name);
            },
            emitPush: function (new_val) {
                this.$emit('push', new_val, this.key_name);
            }
        },
        components: { 'thd': {
            render: function (createElement) {
                return createElement('t' + this.tag, {
                    'class': {
                        'id-col': (this.tag == 'h'),
                        editable: (this.tag == 'd')
                    },
                    on: { click: this.emitClick }
                }, this.$slots.default);
            },
            props: {
                tag: {
                    type: String,
                    required: true
                }
            },
            methods: {
                emitClick: function (event) { this.$emit('click', event); }
            }
        }}
    });

    <?php /* 画面全体の動作を司るVueインスタンス */ ?>
    var test = new Vue({
        el: '#table-form',
        data: {
            column_names: <?=json_encode($tbl_data['col_name'])?>,
            form_config: <?=json_encode($tbl_data['form'])?>,
            tbl_data: <?=json_encode($tbl_data['data'])?>,
            default_data: <?=json_encode($tbl_data['default'])?>,
            modified_form: []
        },
        methods: {
            change: function (new_val, key_name, row_id) {
                Vue.set(this.tbl_data[row_id], key_name, new_val);
                var form_name = key_name + '[' + row_id + ']';
                if (this.modified_form.indexOf(form_name) < 0) {
                    this.modified_form.push(form_name);
                }
            },
            push: function (new_row) {
                var row_id = this.tbl_data.length;
                for (let name in new_row) {
                    this.modified_form.push(name + '[' + row_id + ']');
                }
                this.tbl_data.push(new_row);
            }
        }
    });
</script>
</html>

<?php
// function defineitions

/**
 * @param string $col_name
 * @param array $def_hash
 * @return string
 */
function prtFormTemplate(string $col_name, array $def_hash ) :void {
    $tmpl_str = '<template id="'.$col_name.'">';
    $add_attr = array();
    switch ( $def_hash['type'] ) {
        case 'textarea':
            $tmpl_str .= Html::textarea( 'data['.$col_name.']', $def_hash['default'], $add_attr );
            break;

        case 'select':
            $tmpl_str .= Html::selectbox( 'data['.$col_name.']', $def_hash['ref'], $def_hash['default'], $def_hash['multiple'], $add_attr );
            break;

        case 'multicheck':
            $def_hash['type'] = 'checkbox';
        case 'radio':
            $vals = $def_hash['ref'];
            $chked_vals = $def_hash['ref'];
        case 'checkbox':
            if ( !isset( $vals ) ) {
                $vals = [ 1 => '' ];
            }
            if ( !isset( $chked_vals ) ) {
                $chked_vals = [];
            }
            $tmpl_str .= Html::radiocheck( $def_hash['type'], 'data['.$col_name.']', $vals, $chked_vals, $add_attr );
            break;

        default:
            if ( $def_hash['type'] == 'listext' || $def_hash['type'] == 'numlist' ) {
                $id_attr = uniqid('dl_');
                echo Html::datalist( $id_attr, $def_hash['ref'] );
                $add_attr['list'] = $id_attr;
                if ( $def_hash['type'] == 'listext' ) {
                    $def_hash['type'] = 'text';
                } else if ( $def_hash['type'] == 'numlist' ) {
                    $def_hash['type'] = 'mumber';
                }
            }
            if ( isset( $def_hash['step'] ) ) {
                $add_attr['step'] = $def_hash['step'];
            }
            if ( isset( $def_hash['max'] ) ) {
                $add_attr['max'] = $def_hash['max'];
            }
            if ( isset( $def_hash['step'] ) ) {
                $add_attr['min'] = $def_hash['min'];
            }
            $tmpl_str .= Html::input( $def_hash['type'], 'data['.$col_name.']', $def_hash['default'], $add_attr );
            break;
    }

    $tmpl_str .= '</template>';

    echo $tmpl_str;
}

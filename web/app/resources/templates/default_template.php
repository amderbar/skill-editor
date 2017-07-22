<?php View::obStart('data_table'); ?>
<table class="data-table">
    <thead><tr>
        <th v-for="(conf, id) in column_config"
            v-bind:key="conf.key">{{ conf.col_name || '列' + id }}</th>
    </tr></thead>
    <tbody>
        <tr is="data-row" v-for="(row, idx) in tbl_data"
            :key="idx"
            :idx="idx"
            :data="row"
            :config="column_config"
            :modified_form="modified_form"
            @change="change">
        </tr>
        <tr is="data-row" id="new-rec"
            idx="null"
            :data="default_data"
            :config="column_config"
            @push="push">
        </tr>
    </tbody>
</table>
<?php View::obEnd(); ?>

<?php /* データテーブルの行に関するコンポーネント */?>
<?php View::obStart('script@' . ($tmpl_name ?? '')); ?>
<script type="text/x-template" id="data-row-template">
    <tr>
        <data-cell v-for="(cell, key) in data"
                :key="key"
                :idx="idx"
                :name="key"
                :cell="cell"
                :config="config"
                :modified="modified_form"
                @change="emitChange"
                @push="emitPush">
        </data-cell>
    </tr>
</script><script type="text/javascript">
    Vue.component('data-row', {
        template: '#data-row-template',
        props: ['data', 'idx', 'config', 'modified_form'],
        data: function () { return {
            modified: false,
            focus: null
        }},
        methods: {
            emitChange: function (new_val, key_name) {
                if(this.idx != null) {
                    this.$emit('change', $.extend(this.data, {[key_name]: new_val}), this.idx);
                }
            },
            emitPush: function (new_val, key_name) {
                this.$emit('push', $.extend(this.data, {[key_name]: new_val}));
            }
        }
    });
</script>
<?php View::obStore(); ?>

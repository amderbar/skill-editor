<template>
    <table class="data-table">
        <thead>
            <tr>
                <th v-for="(meta, id) in tblMeta"
                    :key="meta.key"
                >{{ meta.col_name || '列' + id }}</th>
            </tr>
        </thead>
        <tbody>
            <tr v-for="(row_data, idx) in tblData"
                :key="idx">
                <template v-for="(meta, id) in tblMeta">
                    <th v-if="meta.key == 'id'" :key="meta.key" class="id-col">
                        <input type="hidden"
                            :name="`data[${idx}][${meta.key}]`"
                            :value="row_data[meta.key]">
                    </th>
                    <td v-else :key="meta.key">
                        <data-editor-form
                            :type="meta.type"
                            :name="`data[${idx}][${meta.key}]`"
                            :value="row_data[meta.key] || default_data[meta.key]"
                            :options="meta.ref || {}"
                        ></data-editor-form>
                    </td>
                </template>
            </tr>
            <tr id="new-rec">
                <template v-for="(meta, id) in tblMeta">
                    <th v-if="meta.key == 'id'" :key="meta.key" class="id-col">
                        <input type="hidden"
                            :name="`data[null][${meta.key}]`"
                            :value="null">
                    </th>
                    <td v-else :key="meta.key">
                        <data-editor-form
                            :key="meta.key"
                            :type="meta.type"
                            :name="`data[null][${meta.key}]`"
                            :value="default_data[meta.key]"
                            :options="meta.ref || {}"
                        ></data-editor-form>
                    </td>
                </template>
            </tr>
        </tbody>
    </table>
</template>

<script>
export default {
    props: {
        tblData: Array,
        tblMeta: Array,
    },
    data() { return {
        focused: false
    }},
    computed: {
        default_data() {
            let default_data = {};
            this.tblMeta.forEach(function (meta) {
                default_data[meta.key] = meta.default;
            }, this);
            return default_data;
        },
        meta_obj() {
            let meta_obj = {};
            this.tblMeta.forEach(function (meta) {
                meta_obj[meta.key] = meta;
            });
            return meta_obj;
        },
    },
    methods: {
        emitChange(new_val, key_name) {
            if(this.idx != null) {
                this.$emit('change', $.extend(this.data, {[key_name]: new_val}), this.idx);
            }
        },
        emitPush(new_val, key_name) {
            this.$emit('push', $.extend(this.data, {[key_name]: new_val}));
        },
        shiftColumn(event) {
            let shift_step = event.shiftKey ? -1 : 1;
            let $editable = $('.editable');
            let idx = $editable.index($(event.target).closest('.editable'));
            $editable.eq(idx + shift_step).click();
        },
        shiftRow(event) {
            if(!event.shiftKey && event.ctrlKey && this.idx == null) {
                this.emitPush(event.target.value);
                event.target.value = this.cell;
            } else if (event.shiftKey || event.ctrlKey) {
                let shift_step = event.shiftKey ? -1 : 1;
                let $selector = $(`[name^="${this.name}"]`);
                let idx = $selector.index($(event.target).closest('.editable').find(selector));
                this.$nextTick(function () {
                    $selector.eq(idx + shift_step).closest('.editable').click();
                });
            }
        },
    }
}
</script>
